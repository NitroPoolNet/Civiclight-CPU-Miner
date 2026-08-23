/*
 * CivicLight Windows CPU launcher
 *
 * Built as a conservative Core 2-class executable. It checks both CPU
 * feature bits and the SIMD state enabled by Windows before starting an
 * optimized miner binary from the bundled bin directory.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <errno.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#if defined(__GNUC__)
#include <cpuid.h>
#else
#include <intrin.h>
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define CIVICLIGHT_MINER_VERSION L"2.1.0"

typedef struct cpu_features {
    int sse2;
    int ssse3;
    int sse41;
    int sse42;
    int aes;
    int avx;
    int avx2;
    int bmi1;
    int bmi2;
    int fma;
    int movbe;
    int popcnt;
    int sha;
    int avx512;
} cpu_features;

typedef struct miner_build {
    const wchar_t *name;
    const wchar_t *filename;
    const wchar_t *description;
    int (*supported)(const cpu_features *features);
} miner_build;

static void cpuid_query(unsigned int leaf, unsigned int subleaf,
                        unsigned int output[4])
{
#if defined(__GNUC__)
    unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
    __cpuid_count(leaf, subleaf, eax, ebx, ecx, edx);
    output[0] = eax;
    output[1] = ebx;
    output[2] = ecx;
    output[3] = edx;
#else
    int registers[4] = { 0, 0, 0, 0 };
    __cpuidex(registers, (int)leaf, (int)subleaf);
    output[0] = (unsigned int)registers[0];
    output[1] = (unsigned int)registers[1];
    output[2] = (unsigned int)registers[2];
    output[3] = (unsigned int)registers[3];
#endif
}

static uint64_t read_xcr0(void)
{
#if defined(__GNUC__)
    unsigned int eax = 0, edx = 0;
    __asm__ volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
    return ((uint64_t)edx << 32) | eax;
#else
    return _xgetbv(0);
#endif
}

static cpu_features detect_cpu_features(void)
{
    cpu_features features;
    unsigned int registers[4] = { 0, 0, 0, 0 };
    unsigned int max_leaf;
    uint64_t xcr0 = 0;
    int osxsave;
    int cpu_avx;
    int os_avx;
    int os_avx512;

    memset(&features, 0, sizeof(features));
    cpuid_query(0, 0, registers);
    max_leaf = registers[0];

    if (max_leaf < 1)
        return features;

    cpuid_query(1, 0, registers);
    features.sse2 = !!(registers[3] & (1u << 26));
    features.ssse3 = !!(registers[2] & (1u << 9));
    features.fma = !!(registers[2] & (1u << 12));
    features.sse41 = !!(registers[2] & (1u << 19));
    features.sse42 = !!(registers[2] & (1u << 20));
    features.movbe = !!(registers[2] & (1u << 22));
    features.popcnt = !!(registers[2] & (1u << 23));
    features.aes = !!(registers[2] & (1u << 25));
    osxsave = !!(registers[2] & (1u << 27));
    cpu_avx = !!(registers[2] & (1u << 28));

    if (osxsave)
        xcr0 = read_xcr0();

    os_avx = cpu_avx && osxsave && ((xcr0 & 0x6u) == 0x6u);
    os_avx512 = os_avx && ((xcr0 & 0xe6u) == 0xe6u);
    features.avx = os_avx;
    features.fma = features.fma && os_avx;

    if (max_leaf >= 7) {
        cpuid_query(7, 0, registers);
        features.bmi1 = !!(registers[1] & (1u << 3));
        features.avx2 = !!(registers[1] & (1u << 5)) && os_avx;
        features.bmi2 = !!(registers[1] & (1u << 8));
        features.sha = !!(registers[1] & (1u << 29));
        features.avx512 = os_avx512 &&
            ((registers[1] & ((1u << 16) | (1u << 17) | (1u << 28) |
                              (1u << 30) | (1u << 31))) ==
             ((1u << 16) | (1u << 17) | (1u << 28) |
              (1u << 30) | (1u << 31)));
    }

    return features;
}

static int supports_core2(const cpu_features *f)
{
    return f->sse2 && f->ssse3;
}

static int supports_avx(const cpu_features *f)
{
    return supports_core2(f) && f->avx && f->aes && f->sse41 &&
           f->sse42 && f->popcnt;
}

static int supports_avx2(const cpu_features *f)
{
    return supports_avx(f) && f->avx2 && f->bmi1 && f->bmi2 && f->fma &&
           f->movbe;
}

static int supports_avx2_sha(const cpu_features *f)
{
    return supports_avx2(f) && f->sha;
}

static int supports_avx512(const cpu_features *f)
{
    return supports_avx2(f) && f->avx512;
}

static int supports_avx512_sha(const cpu_features *f)
{
    return supports_avx512(f) && f->sha;
}

/* Fastest build first for automatic selection. */
static const miner_build builds[] = {
    { L"avx512-sha", L"civiclight-avx512-sha.exe", L"AVX-512 + SHA-NI", supports_avx512_sha },
    { L"avx512",     L"civiclight-avx512.exe",     L"AVX-512",          supports_avx512 },
    { L"avx2-sha",   L"civiclight-avx2-sha.exe",   L"AVX2 + SHA-NI",    supports_avx2_sha },
    { L"avx2",       L"civiclight-avx2.exe",       L"AVX2",             supports_avx2 },
    { L"avx",        L"civiclight-avx.exe",        L"AVX",              supports_avx },
    { L"core2",      L"civiclight-core2.exe",      L"Core 2 baseline",  supports_core2 }
};

static int file_exists(const wchar_t *path)
{
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

static int get_launcher_directory(wchar_t *directory, size_t capacity)
{
    DWORD length = GetModuleFileNameW(NULL, directory, (DWORD)capacity);
    wchar_t *separator;

    if (length == 0 || length >= capacity)
        return 0;

    separator = wcsrchr(directory, L'\\');
    if (separator == NULL)
        return 0;

    *separator = L'\0';
    return 1;
}

static int build_miner_path(wchar_t *path, size_t capacity,
                            const wchar_t *directory,
                            const wchar_t *filename)
{
    int result = swprintf(path, capacity, L"%ls\\bin\\%ls",
                          directory, filename);
    if (result < 0 || (size_t)result >= capacity) {
        if (capacity > 0)
            path[capacity - 1] = L'\0';
        return 0;
    }
    return 1;
}

static int wide_equals_ignore_case(const wchar_t *left, const wchar_t *right)
{
    while (*left != L'\0' && *right != L'\0') {
        if (towlower(*left) != towlower(*right))
            return 0;
        ++left;
        ++right;
    }
    return *left == *right;
}

static void get_cpu_brand(wchar_t *brand, size_t capacity)
{
    unsigned int registers[4] = { 0, 0, 0, 0 };
    char raw_brand[49];
    unsigned int max_extended;
    int i;

    if (capacity == 0)
        return;

    brand[0] = L'\0';
    memset(raw_brand, 0, sizeof(raw_brand));
    cpuid_query(0x80000000u, 0, registers);
    max_extended = registers[0];
    if (max_extended < 0x80000004u)
        return;

    for (i = 0; i < 3; ++i) {
        cpuid_query(0x80000002u + (unsigned int)i, 0, registers);
        memcpy(raw_brand + (i * 16), registers, 16);
    }

    while (raw_brand[0] == ' ')
        memmove(raw_brand, raw_brand + 1, strlen(raw_brand));

    {
        size_t length = strlen(raw_brand);
        while (length > 0 && raw_brand[length - 1] == ' ')
            raw_brand[--length] = '\0';
    }

    MultiByteToWideChar(CP_ACP, 0, raw_brand, -1, brand, (int)capacity);
    brand[capacity - 1] = L'\0';
}

static const miner_build *find_named_build(const wchar_t *name)
{
    size_t i;
    for (i = 0; i < ARRAY_SIZE(builds); ++i) {
        if (wide_equals_ignore_case(name, builds[i].name))
            return &builds[i];
    }
    return NULL;
}

static void print_launcher_help(void)
{
    wprintf(L"CivicLight Windows automatic CPU launcher\n\n");
    wprintf(L"Launcher options:\n");
    wprintf(L"  --cpu-build NAME       select a compatible build manually\n");
    wprintf(L"  --list-cpu-builds      show CPU compatibility and bundled files\n");
    wprintf(L"  --launcher-dry-run     display the selected build without starting it\n");
    wprintf(L"  --launcher-help        display this launcher help\n\n");
    wprintf(L"All other options are passed unchanged to the miner.\n");
}

static void print_feature_summary(const cpu_features *f)
{
    wprintf(L"Detected features:");
    if (f->sse2) wprintf(L" SSE2");
    if (f->ssse3) wprintf(L" SSSE3");
    if (f->aes) wprintf(L" AES");
    if (f->avx) wprintf(L" AVX");
    if (f->avx2) wprintf(L" AVX2");
    if (f->sha) wprintf(L" SHA-NI");
    if (f->avx512) wprintf(L" AVX-512");
    wprintf(L"\n");
}

int wmain(int argc, wchar_t **argv)
{
    cpu_features features = detect_cpu_features();
    const miner_build *selected = NULL;
    const wchar_t *requested_name = NULL;
    wchar_t launcher_directory[MAX_PATH];
    wchar_t miner_path[MAX_PATH];
    wchar_t cpu_brand[64];
    wchar_t **child_argv;
    int child_argc = 1;
    int list_builds = 0;
    int dry_run = 0;
    int i;
    intptr_t result;

    if (!get_launcher_directory(launcher_directory, ARRAY_SIZE(launcher_directory))) {
        fwprintf(stderr, L"ERROR: unable to locate the CivicLight launcher directory.\n");
        return 2;
    }

    child_argv = (wchar_t **)calloc((size_t)argc + 1, sizeof(*child_argv));
    if (child_argv == NULL) {
        fwprintf(stderr, L"ERROR: unable to allocate launcher arguments.\n");
        return 2;
    }

    for (i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"--launcher-help") == 0) {
            print_launcher_help();
            free(child_argv);
            return 0;
        } else if (wcscmp(argv[i], L"--list-cpu-builds") == 0) {
            list_builds = 1;
        } else if (wcscmp(argv[i], L"--launcher-dry-run") == 0) {
            dry_run = 1;
        } else if (wcscmp(argv[i], L"--cpu-build") == 0) {
            if (++i >= argc) {
                fwprintf(stderr, L"ERROR: --cpu-build requires a build name.\n");
                free(child_argv);
                return 2;
            }
            requested_name = argv[i];
        } else if (wcsncmp(argv[i], L"--cpu-build=", 12) == 0) {
            requested_name = argv[i] + 12;
        } else {
            child_argv[child_argc++] = argv[i];
        }
    }

    get_cpu_brand(cpu_brand, ARRAY_SIZE(cpu_brand));
    wprintf(L"CivicLight Windows CPU Miner v%ls\n",
            CIVICLIGHT_MINER_VERSION);
    if (cpu_brand[0] != L'\0')
        wprintf(L"Detected CPU: %ls\n", cpu_brand);
    print_feature_summary(&features);

    if (list_builds) {
        size_t index;
        wprintf(L"\nAvailable CPU builds:\n");
        for (index = 0; index < ARRAY_SIZE(builds); ++index) {
            int installed;
            build_miner_path(miner_path, ARRAY_SIZE(miner_path),
                             launcher_directory, builds[index].filename);
            installed = file_exists(miner_path);
            wprintf(L"  %-11ls  CPU: %-11ls  File: %ls\n",
                    builds[index].name,
                    builds[index].supported(&features) ? L"supported" : L"unsupported",
                    installed ? L"present" : L"missing");
        }
        free(child_argv);
        return 0;
    }

    if (requested_name != NULL) {
        selected = find_named_build(requested_name);
        if (selected == NULL) {
            fwprintf(stderr, L"ERROR: unknown CPU build '%ls'.\n", requested_name);
            fwprintf(stderr, L"Use --list-cpu-builds to list valid names.\n");
            free(child_argv);
            return 2;
        }
        if (!selected->supported(&features)) {
            fwprintf(stderr,
                     L"ERROR: the '%ls' build requires instructions not available on this CPU or enabled by Windows.\n",
                     selected->name);
            free(child_argv);
            return 2;
        }
        if (!build_miner_path(miner_path, ARRAY_SIZE(miner_path),
                              launcher_directory, selected->filename) ||
            !file_exists(miner_path)) {
            fwprintf(stderr, L"ERROR: requested miner binary is missing: %ls\n",
                     selected->filename);
            free(child_argv);
            return 2;
        }
    } else {
        size_t index;
        for (index = 0; index < ARRAY_SIZE(builds); ++index) {
            if (!builds[index].supported(&features))
                continue;
            if (!build_miner_path(miner_path, ARRAY_SIZE(miner_path),
                                  launcher_directory, builds[index].filename))
                continue;
            if (file_exists(miner_path)) {
                selected = &builds[index];
                break;
            }
        }
    }

    if (selected == NULL) {
        fwprintf(stderr,
                 L"ERROR: no compatible CivicLight miner binary was found.\n");
        fwprintf(stderr,
                 L"This release requires a 64-bit Core 2-class CPU with SSE2 and SSSE3.\n");
        free(child_argv);
        return 2;
    }

    wprintf(L"Selected build: %ls (%ls)\n\n", selected->name,
            selected->description);
    fflush(stdout);

    if (dry_run) {
        wprintf(L"Launcher dry run complete; miner was not started.\n");
        free(child_argv);
        return 0;
    }

    child_argv[0] = (wchar_t *)selected->filename;
    child_argv[child_argc] = NULL;
    result = _wspawnv(_P_WAIT, miner_path, (const wchar_t * const *)child_argv);
    if (result == -1) {
        fwprintf(stderr, L"ERROR: unable to start %ls (errno %d).\n",
                 selected->filename, errno);
        free(child_argv);
        return 2;
    }

    free(child_argv);
    return (int)result;
}

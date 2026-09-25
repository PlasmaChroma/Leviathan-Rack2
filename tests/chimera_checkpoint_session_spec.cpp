#include "ChimeraCheckpointSession.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>
static void need(bool ok, const char* message) {
    if (!ok) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
int main(int argc, char** argv) {
    if (argc == 3) {
        chimera::CheckpointSession child;
        need(child.ensure(argv[2]), "child lease");
        { std::ofstream file(child.directory() + "/reel-1-1.wav"); file << "crash"; }
        std::_Exit(0); // Real process exit: no destructor releases this lease.
    }
    const std::string root = "build/tests/checkpoint-session-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto later = std::time(nullptr) + 172800;
    std::string abandoned;
    {
        chimera::CheckpointSession a, b;
        need(a.ensure(root) && b.ensure(root) && a.directory() != b.directory(), "unique live sessions");
        abandoned = a.directory();
        { std::ofstream file(abandoned + "/reel-1-2.wav"); file << "undo"; }
        need(chimera::CheckpointSession::sweep(root, later) == 0 &&
             rack::system::isFile(abandoned + "/reel-1-2.wav"), "age never deletes leased Undo");
    }
    need(chimera::CheckpointSession::sweep(root, std::time(nullptr)) == 0, "abandoned session grace period");
    need(chimera::CheckpointSession::sweep(root, later) == 2 && !rack::system::exists(abandoned),
         "unleased sessions are reclaimed after grace");
#ifdef _WIN32
    wchar_t executable[32768];
    need(GetModuleFileNameW(nullptr, executable, 32768) != 0, "test executable path");
    std::wstring command = L"\"" + std::wstring(executable) + L"\" crash \"" +
        rack::string::UTF8toUTF16(root) + L"\"";
    STARTUPINFOW startup{}; startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    need(CreateProcessW(executable, &command[0], nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
        nullptr, nullptr, &startup, &process), "start crash fixture process");
    need(WaitForSingleObject(process.hProcess, 10000) == WAIT_OBJECT_0, "crash process deadline");
    DWORD code = 1; GetExitCodeProcess(process.hProcess, &code);
    CloseHandle(process.hProcess); CloseHandle(process.hThread);
    need(code == 0, "crash fixture exit");
#else
    const std::string command = "\"" + std::string(argv[0]) + "\" crash \"" + root + "\"";
    need(std::system(command.c_str()) == 0, "crash fixture process");
#endif
    need(chimera::CheckpointSession::sweep(root, later) == 1, "OS releases lease after abrupt process exit");
    std::string unknown;
    {
        chimera::CheckpointSession session;
        need(session.ensure(root), "unknown-file fixture");
        unknown = session.directory();
        { std::ofstream file(unknown + "/user.txt"); file << "keep"; }
    }
    need(chimera::CheckpointSession::sweep(root, later) == 0 &&
         rack::system::isFile(unknown + "/user.txt"), "unknown contents are never removed");
    rack::system::remove(unknown + "/user.txt");
    need(chimera::CheckpointSession::sweep(root, later) == 1, "cleanup retries safely");
#ifdef _WIN32
    std::string blocked;
    {
        chimera::CheckpointSession session;
        need(session.ensure(root), "failed deletion fixture");
        blocked = session.directory();
        { std::ofstream file(blocked + "/reel-2-3.wav"); file << "undo"; }
    }
    chimera::CheckpointLease fileLease;
    need(fileLease.open(blocked + "/reel-2-3.wav"), "hold checkpoint against deletion");
    need(chimera::CheckpointSession::sweep(root, later) == 0 &&
         rack::system::isFile(blocked + "/owner.lock"), "failed unlink retains lease file for retry");
    fileLease.close();
    need(chimera::CheckpointSession::sweep(root, later) == 1, "failed unlink retries on later sweep");
#endif
    {
        // Unknown files cannot starve valid sessions when callers alternate
        // between the undo and save-staging roots on every bounded sweep.
        const std::string roots[2] = {root + "-undo", root + "-save"};
        for (const auto& directory : roots) {
            rack::system::createDirectories(directory);
            for (unsigned i = 0; i < 512; ++i) {
                std::ofstream file(directory + "/000-unknown-" + std::to_string(i));
                file << "keep";
            }
            for (unsigned i = 0; i < 32; ++i) {
                chimera::CheckpointSession session;
                need(session.ensure(directory), "multi-root session");
                std::ofstream file(session.directory() + "/reel-1-2.json");
                file << "staged manifest";
            }
        }
        unsigned removed[2]{};
        for (unsigned pass = 0; pass < 40; ++pass)
            for (unsigned i = 0; i < 2; ++i)
                removed[i] += chimera::CheckpointSession::sweep(roots[i], later);
        need(removed[0] == 32 && removed[1] == 32, "alternating roots preserve bounded scan progress");
        for (const auto& directory : roots) {
            need(rack::system::isFile(directory + "/000-unknown-511"), "sweeps retain unknown files");
            rack::system::removeRecursively(directory);
        }
    }
    rack::system::remove(root + "/owner.lock");
    rack::system::remove(root);
    std::cout << "PASS: checkpoint sessions, live leases, grace period, crash and unknown-file retention\n";
}

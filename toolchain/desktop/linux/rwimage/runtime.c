/*
 * RWImage self-executing runtime.
 *
 * Locates the SquashFS appended to this ELF, mounts it via squashfuse in a
 * child process, runs launch.sh from the mountpoint, then unmounts and exits
 * with the application's exit code.
 *
 * Dependencies: libfuse.so.2 (system), libsquashfuse.a + libzstd.a (vcpkg, static)
 */

/*
 * Define FUSE_USE_VERSION explicitly rather than pulling in squashfuse/config.h.
 * config.h unconditionally #defines _POSIX_C_SOURCE 200112L, which conflicts
 * with the _POSIX_C_SOURCE already set by system headers on some toolchains.
 * We know from the installed config.h that squashfuse was built against FUSE 2.
 */
#define FUSE_USE_VERSION 26

#include <squashfuse/ll.h>
#include <squashfuse/fuseprivate.h>

#include <elf.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* SquashFS superblock magic (little-endian and big-endian variants) */
static const uint32_t SQFS_MAGIC_LE = 0x73717368u; /* "hsqs" in LE memory */
static const uint32_t SQFS_MAGIC_BE = 0x68737173u; /* "sqsh" in BE memory */

static void die(const char *msg)
{
    perror(msg);
    exit(1);
}

/* lseek+read pair; safe here because all scanning is single-threaded at startup */
static ssize_t read_at(int fd, void *buf, size_t count, off_t offset)
{
    if (lseek(fd, offset, SEEK_SET) < 0)
        return -1;
    return read(fd, buf, count);
}

/*
 * Return the byte offset at which the ELF file data ends (i.e. where
 * appended non-ELF content begins).  Reads both program and section
 * headers to find the highest used file offset.
 */
static size_t elf_end(int fd)
{
    unsigned char ident[EI_NIDENT];
    if (read_at(fd, ident, EI_NIDENT, 0) != EI_NIDENT)
        return 0;
    if (memcmp(ident, ELFMAG, SELFMAG) != 0)
        return 0;

    size_t end = 0;

    if (ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr h;
        if (read_at(fd, &h, sizeof h, 0) != (ssize_t)sizeof h)
            return 0;
        for (uint16_t i = 0; i < h.e_phnum; i++) {
            Elf64_Phdr ph;
            off_t off = (off_t)(h.e_phoff + (size_t)i * h.e_phentsize);
            if (read_at(fd, &ph, sizeof ph, off) == (ssize_t)sizeof ph) {
                size_t e = ph.p_offset + ph.p_filesz;
                if (e > end) end = e;
            }
        }
        if (h.e_shoff) {
            size_t e = h.e_shoff + (size_t)h.e_shnum * h.e_shentsize;
            if (e > end) end = e;
        }
    } else {
        Elf32_Ehdr h;
        if (read_at(fd, &h, sizeof h, 0) != (ssize_t)sizeof h)
            return 0;
        for (uint16_t i = 0; i < h.e_phnum; i++) {
            Elf32_Phdr ph;
            off_t off = (off_t)(h.e_phoff + (size_t)i * h.e_phentsize);
            if (read_at(fd, &ph, sizeof ph, off) == (ssize_t)sizeof ph) {
                size_t e = ph.p_offset + ph.p_filesz;
                if (e > end) end = e;
            }
        }
        if (h.e_shoff) {
            size_t e = h.e_shoff + (size_t)h.e_shnum * h.e_shentsize;
            if (e > end) end = e;
        }
    }

    return end;
}

/*
 * Scan for SquashFS magic starting at min_offset, in 4-byte steps.
 * Returns the offset on success, -1 if not found.
 *
 * We start past the ELF binary to avoid false positives from squashfuse's
 * own SQUASHFS_MAGIC constant embedded in the linked library code.
 */
static ssize_t find_squashfs_offset(int fd, size_t min_offset)
{
    char buf[65536];
    size_t pos = min_offset;
    ssize_t n;

    while ((n = read_at(fd, buf, sizeof buf, (off_t)pos)) > 0) {
        for (ssize_t i = 0; i + 4 <= n; i += 4) {
            uint32_t magic;
            memcpy(&magic, buf + i, 4);
            if (magic == SQFS_MAGIC_LE || magic == SQFS_MAGIC_BE)
                return (ssize_t)(pos + (size_t)i);
        }
        pos += (size_t)n;
        if (n < (ssize_t)sizeof buf)
            break;
    }
    return -1;
}

static struct fuse_lowlevel_ops sqfs_ops = {
    .getattr    = sqfs_ll_op_getattr,
    .opendir    = sqfs_ll_op_opendir,
    .create     = sqfs_ll_op_create,
    .releasedir = sqfs_ll_op_releasedir,
    .readdir    = sqfs_ll_op_readdir,
    .lookup     = sqfs_ll_op_lookup,
    .open       = sqfs_ll_op_open,
    .release    = sqfs_ll_op_release,
    .read       = sqfs_ll_op_read,
    .readlink   = sqfs_ll_op_readlink,
    .listxattr  = sqfs_ll_op_listxattr,
    .getxattr   = sqfs_ll_op_getxattr,
    .forget     = sqfs_ll_op_forget,
    .statfs     = stfs_ll_op_statfs,
};

int main(int argc, char *argv[])
{
    /* Resolve own path (argv[0] may be relative or from PATH) */
    char self_path[PATH_MAX];
    ssize_t self_len = readlink("/proc/self/exe", self_path, sizeof self_path - 1);
    if (self_len < 0)
        die("readlink /proc/self/exe");
    self_path[self_len] = '\0';

    /* Find the SquashFS offset by reading past the ELF data */
    int self_fd = open(self_path, O_RDONLY);
    if (self_fd < 0)
        die("open self");
    size_t elf_size = elf_end(self_fd);
    ssize_t sqfs_offset = find_squashfs_offset(self_fd, elf_size);
    close(self_fd);

    if (sqfs_offset < 0) {
        fprintf(stderr, "rwimage: no SquashFS image found in %s\n", self_path);
        return 1;
    }

    /* Create a unique temporary mount directory */
    char mountpoint[64];
    snprintf(mountpoint, sizeof mountpoint, "/tmp/.rwimage-%d", (int)getpid());
    if (mkdir(mountpoint, 0700) != 0)
        die("mkdir mountpoint");

    /*
     * Use a pipe to synchronise: FUSE child writes 'R' once the mount is
     * established, parent blocks on read before launching the application.
     */
    int ready[2];
    if (pipe(ready) != 0)
        die("pipe");

    pid_t fuse_pid = fork();
    if (fuse_pid < 0)
        die("fork fuse");

    if (fuse_pid == 0) {
        /* --- FUSE child --- */
        close(ready[0]);

        sqfs_ll *ll = sqfs_ll_open(self_path, (size_t)sqfs_offset);
        if (!ll) {
            fprintf(stderr, "rwimage: failed to open SquashFS at offset %zd\n",
                    sqfs_offset);
            write(ready[1], "E", 1);
            exit(1);
        }

        char *fuse_argv[] = { "rwimage", "-o", "ro", NULL };
        struct fuse_args fuse_args = FUSE_ARGS_INIT(3, fuse_argv);

        sqfs_ll_chan ch;
        if (sqfs_ll_mount(&ch, mountpoint, &fuse_args,
                          &sqfs_ops, sizeof sqfs_ops, ll) != SQFS_OK) {
            fprintf(stderr, "rwimage: FUSE mount failed\n");
            write(ready[1], "E", 1);
            sqfs_ll_destroy(ll);
            exit(1);
        }
        fuse_opt_free_args(&fuse_args);

        /* Mount is live; unblock parent */
        write(ready[1], "R", 1);
        close(ready[1]);

        /*
         * Install libfuse's default signal handlers so that SIGTERM/SIGINT
         * causes fuse_session_loop to return cleanly, allowing us to call
         * sqfs_ll_unmount before exiting.
         */
        fuse_set_signal_handlers(ch.session);
        fuse_session_loop(ch.session);
        fuse_remove_signal_handlers(ch.session);

        sqfs_ll_unmount(&ch, mountpoint);
        sqfs_ll_destroy(ll);
        exit(0);
    }

    /* --- Parent --- */
    close(ready[1]);

    char status = 0;
    if (read(ready[0], &status, 1) != 1 || status != 'R') {
        fprintf(stderr, "rwimage: FUSE child failed to start\n");
        waitpid(fuse_pid, NULL, 0);
        rmdir(mountpoint);
        return 1;
    }
    close(ready[0]);

    /* Export image metadata for the launch script and the application */
    setenv("RWIMAGE", self_path, 1);
    setenv("RWIMAGE_MOUNT", mountpoint, 1);

    /* Build argv for the launch script, forwarding all original arguments */
    char apprun[PATH_MAX];
    snprintf(apprun, sizeof apprun, "%s/launch.sh", mountpoint);

    char **child_argv = malloc(((size_t)argc + 1) * sizeof(char *));
    if (!child_argv)
        die("malloc");
    child_argv[0] = apprun;
    for (int i = 1; i < argc; i++)
        child_argv[i] = argv[i];
    child_argv[argc] = NULL;

    pid_t app_pid = fork();
    if (app_pid < 0)
        die("fork app");
    if (app_pid == 0) {
        execv(apprun, child_argv);
        perror("execv launch.sh");
        exit(127);
    }
    free(child_argv);

    /* Wait for the application to finish */
    int app_status = 0;
    waitpid(app_pid, &app_status, 0);

    /* Ask the FUSE daemon to unmount cleanly, then wait for it */
    kill(fuse_pid, SIGTERM);
    waitpid(fuse_pid, NULL, 0);
    rmdir(mountpoint);

    /* Mirror the application's exit behaviour */
    if (WIFEXITED(app_status))
        return WEXITSTATUS(app_status);
    if (WIFSIGNALED(app_status))
        kill(getpid(), WTERMSIG(app_status));
    return 1;
}

/*
 * ISC License
 *
 * Copyright (c) 2022 Erich Ericson
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
//#define SLEEPTIME 50000000
/* fork sigaction kill wait exec sigprocmask */
static void
sleeps(const long stime)
{
    const struct timespec times = { 0, stime };
    nanosleep(&times, NULL);
}
int
systeml(const char * command, const long len)
{
    pid_t pid;
    sigset_t oldmask;
    sigset_t newmask;
    const struct sigaction sigact = { SIG_IGN, 0, 0, 0, 0 } ;
    struct sigaction oldact;

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGCHLD);
    sigprocmask(SIG_SETMASK, &newmask, &oldmask);
    sigaction(SIGINT, &sigact, &oldact);
    sigaction(SIGQUIT, &sigact, NULL);
    pid = fork();
    if (pid == -1) {
        printf("Error forking!\n");
        exit(1);
    }
    if (pid == 0) {
        /* child */
        execl("/bin/sh","sh", "-c", command, (char*) NULL);
    }
    if (pid != 0) {
        /* parent */
        int wstatus = 1;
        int waitpids = 0;

        sigprocmask(SIG_SETMASK, &oldmask, NULL); 
        sigaction(SIGINT, &oldact, NULL);
        sigaction(SIGQUIT, &oldact, NULL);
        sleeps(len);
        waitpids = waitpid(pid, &wstatus, WNOHANG);
        if (waitpids == 0){
            kill(pid, SIGTERM);
        } else if (waitpids != -1) {
            return WEXITSTATUS(wstatus);
        }
        sleeps(len);
        waitpids = waitpid(pid, &wstatus, WNOHANG);
        if (waitpids == 0) {
            kill(pid, SIGKILL);
        } else if (waitpids != -1) {
            return WEXITSTATUS(wstatus);
        }
    }        
    return -1;
}

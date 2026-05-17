#include "trace_runtime.h"

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error "Este runtime didatico suporta apenas Linux x86_64."
#endif

static void fill_event_from_regs(pid_t pid,
                                 int entering,
                                 const struct user_regs_struct *regs,
                                 struct syscall_event *ev)
{
    /*
     * TODO Semana 4:
     *
     * Preencha struct syscall_event usando os registradores x86_64.
     *
     * Dicas:
     * - regs->orig_rax contem o numero da syscall.
     * - regs->rax contem o retorno, valido na saida.
     * - os seis argumentos ficam em rdi, rsi, rdx, r10, r8 e r9.
     * - ev->entering deve copiar o parametro entering.
     */
    memset(ev, 0, sizeof(*ev));
    ev->pid = pid;
    ev->entering = entering;
}

static pid_t launch_tracee(char *const argv[])
{
    /*
     * TODO Semana 2: - FEITO
     *
     * Crie o processo monitorado.
     *
     * Fluxo esperado:
     * - fork()
     * - no filho:
     *   - ptrace(PTRACE_TRACEME, ...)
     *   - raise(SIGSTOP)
     *   - execvp(argv[0], argv)
     * - no pai:
     *   - retornar o pid do filho
     *
     * Em erro, imprima uma mensagem com perror() e retorne -1.
     */

    pid_t child = fork();

    if (child == -1) {
        perror("Erro: Fork Falho\n");
        return -1;
    }

    if (child == 0) {
        /* --- Filho --- */
        
        /* Passagem de Parâmetros [Requisição, Pid, Addr, Data] */
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("Erro no ptrace: TRACEME\n");
            exit(EXIT_FAILURE);
        }

        /* Filho levanta a flag para Pai configurar a trace */
        raise(SIGSTOP);

        execvp(argv[0], argv);

        /* Execvp Falha */
        perror("Erro no execvp\n");
        exit(EXIT_FAILURE);
    }

    /* --- Pai --- */
    return child;
}

static int wait_for_initial_stop(pid_t child)
{
    /*
     * TODO Semana 2: - FEITO
     *
     * O filho chama raise(SIGSTOP) antes de executar o programa alvo.
     * O pai precisa esperar essa parada inicial com waitpid().
     *
     * Retorne 0 se o filho parou como esperado, -1 em erro.
     */

    int status;

    /* Aguarda a mudança do Filho */
    if (waitpid(child, &status, 0) == -1) {
        perror("Erro: waitpid inicial\n");
        return -1;
    }

    /* Confirmação da Pausa */
    if (WIFSTOPPED(status)) {
        return 0;
    }

    fprintf(stderr, "Erro: filho não parou no SIGSTOP\n");
    return -1;
}

static int configure_trace_options(pid_t child)
{
    /*
     * TODO Semana 3: - FEITO
     *
     * Configure PTRACE_O_TRACESYSGOOD com PTRACE_SETOPTIONS.
     * Isso ajuda a diferenciar paradas de syscall de outros sinais.
     */

    /* Passagem de Parâmetros [Requisição, Pid, Addr, Data] */
    if (ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACESYSGOOD) < 0) {
        perror("Erro: ptrace SETOPTIONS");
        return -1;
    }
    return 0;
}

static int resume_until_next_syscall(pid_t child, int signal_to_deliver)
{
    /*
     * TODO Semana 3:
     *
     * Use ptrace(PTRACE_SYSCALL, ...) para deixar o filho executar ate a
     * proxima entrada ou saida de syscall.
     *
     * signal_to_deliver deve ser repassado como quarto argumento do ptrace.
     */

    /* Passagem de Parâmetros [Requisição, Pid, Addr, Data] */
    if (ptrace(PTRACE_SYSCALL, child, NULL, signal_to_deliver) < 0) {
        perror("Erro: ptrace SYSCALL");
        return -1;
    }
    return 0;
}

static int wait_for_syscall_stop(pid_t child, int *status)
{
    /*
     * TODO Semana 3: - FEITO
     *
     * Espere o filho com waitpid().
     *
     * Retorne:
     *   1 se a parada foi uma parada de syscall;
     *   0 se o filho terminou normalmente ou por sinal;
     *  -1 em erro.
     *
     * Dicas:
     * - WIFEXITED e WIFSIGNALED indicam fim do processo.
     * - WIFSTOPPED indica que o processo parou.
     * - com PTRACE_O_TRACESYSGOOD, syscall-stops aparecem com bit 0x80.
     * - paradas SIGTRAP comuns nao devem ser entregues de volta ao filho.
     */

    // OBS: Não sei se esse while intermitente seria a melhor opção, mas não consigo pensar em outro jeito
    while (1) {
        if (waitpid(child, status, 0) < 0) {
            perror("Erro: Loop de waitpid");
            return -1;
        }

        /* Verifica se o processo encerrou a execução */
        if (WIFEXITED(*status) || WIFSIGNALED(*status)) {
            return 0;
        }

        if (WIFSTOPPED(*status)) {
            int sig = WSTOPSIG(*status);

            /* Identifica se a parada foi causada por uma syscall */
            if (sig & 0x80) {
                return 1;
            }

            /* Ignora paradas SIGTRAP e repassa outros sinais */
            int next_signal = (sig == SIGTRAP) ? 0 : sig;
            
            if (ptrace(PTRACE_SYSCALL, child, NULL, next_signal) < 0) {
                perror("ptrace re-envio");
                return -1;
            }
        }
    }
}

int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata)
{
    pid_t child;
    int status = 0;
    int entering = 1;

    if (argv == NULL || argv[0] == NULL) {
        fprintf(stderr, "erro: programa alvo ausente\n");
        return -1;
    }

    child = launch_tracee(argv);
    if (child < 0) {
        return -1;
    }

    if (wait_for_initial_stop(child) < 0) {
        return -1;
    }

    if (configure_trace_options(child) < 0) {
        return -1;
    }

    if (resume_until_next_syscall(child, 0) < 0) {
        return -1;
    }

    while (1) {
        struct user_regs_struct regs;
        struct syscall_event ev;
        int stop_kind;

        stop_kind = wait_for_syscall_stop(child, &status);
        if (stop_kind < 0) {
            return -1;
        }
        if (stop_kind == 0) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 0;
        }

        /*
         * TODO Semana 4:
         *
         * Use PTRACE_GETREGS para preencher regs.
         * Depois chame fill_event_from_regs() e observer().
         */
        memset(&regs, 0, sizeof(regs));
        fill_event_from_regs(child, entering, &regs, &ev);
        if (observer != NULL) {
            observer(&ev, userdata);
        }

        entering = !entering;

        if (resume_until_next_syscall(child, 0) < 0) {
            return -1;
        }
    }
}

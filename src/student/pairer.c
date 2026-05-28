#include "student_api.h"
#include "trace_helpers.h"
#include <string.h>
#include <sys/syscall.h>
#include <stdlib.h>

int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{
    /*
     * TODO Semana 5:
     *
     * O runtime chama esta funcao duas vezes para cada syscall:
     *
     *   1. uma vez antes da syscall executar
     *   2. uma vez depois da syscall terminar
     *
     * Na primeira parada, os argumentos estao disponiveis.
     * Na segunda parada, o retorno esta disponivel.
     *
     * Seu trabalho e produzir um evento completo apenas quando ja existirem
     * as duas metades da syscall.
     *
     * Dicas:
     * - ev->entering == 1 indica entrada de syscall.
     * - ev->entering == 0 indica saida de syscall.
     * - para comecar, assuma apenas um processo monitorado.
     *
     * Retorne:
     *   1 se out contem uma syscall completa
     *   0 se ainda nao ha syscall completa
     *  -1 se a sequencia de eventos parece invalida
     */
   if (ev->entering) {
      pairer->entry = *ev;
      pairer->has_entry = 1;

      /* 
       * Em vez de variaveis globais ou sobrescrever o array e truncar em 48 bytes,
       * alocamos um buffer de 256 bytes e salvamos o PONTEIRO na posicao args[3] (que e ociosa).
       */
      if (ev->syscall_no == SYS_execve) {
          char *path_payload = malloc(256);
          if (path_payload) {
              if (read_child_string(ev->pid, ev->args[0], path_payload, 256) != 0) {
                  strncpy(path_payload, "<ilegivel>", 256);
              }
              pairer->entry.args[3] = (unsigned long)path_payload;
          }
      }

      if (ev->syscall_no == SYS_exit_group) {
          *out = pairer->entry;
          out->entering = 0;
          pairer->has_entry = 0;
          return 1;
      }

      return 0;
  }

  if (pairer->has_entry && pairer->entry.syscall_no == ev->syscall_no) {
    *out = pairer->entry;
    out->ret = ev->ret;
    out->entering = 0;
    pairer->has_entry = 0;
    return 1;
  }

  return -1;
}

#include "student_api.h"

int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{
    /*
     * TODO Semana 2:
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
        /* Evento de entrada: armazena para parear depois */
        pairer->entry = *ev;
        pairer->has_entry = 1;
        return 0;
    }

    /* Evento de saída */
    if (!pairer->has_entry) {
        /* Saída sem entrada correspondente: sequência inválida */
        return -1;
    }

    /* Monta o evento completo: argumentos da entrada + retorno da saída */
    *out = pairer->entry;   /* copia tudo da entrada (args, syscall_no, pid) */
    out->entering = 0;      /* marca como evento completo (saída) */
    out->ret = ev->ret;     /* preenche o retorno capturado na saída */

    pairer->has_entry = 0;  /* reseta para a próxima syscall */
    return 1;
}

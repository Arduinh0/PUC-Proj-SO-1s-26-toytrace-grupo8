# Mapa do Código - Toytrace (Grupo 8)

## 1. Onde o programa começa?
O programa começa no arquivo `src/main.c`, na função `main()`. É lá que a ferramenta lê os argumentos do terminal (usando a CLI) e decide se vai imprimir a ajuda ou iniciar a função de rastreamento (tracing).

## 2. Onde o processo alvo é criado?
No arquivo `src/trace_runtime.c`, especificamente dentro da função `launch_tracee()`. É dentro dessa função que (na *Semana 2*) vamos implementar o `fork()` para criar um processo filho e - em seguida - utilizar o `execvp()` para rodar o programa alvo.

## 3. Onde o runtime chama o callback?
O callback é chamado no loop principal de monitoramento do pai, também dentro de `src/trace_runtime.c`. Isso acontece logo após o `waitpid` avisar que o processo filho parou por causa de uma syscall, onde o pai então usa `PTRACE_GETREGS` para ler os registradores, monta a `struct syscall_event` e a envia pelo callback.

## 4. Quais arquivos o grupo deve modificar?
Nosso trabalho vai se concentrar em duas frentes:
1. Os "TODO's" deixados no arquivo `src/trace_runtime.c` (para criar o processo e configurar o ptrace).
2. Os arquivos dentro da pasta `src/student/` (como `formatter.c`, `pairer.c`, etc.), onde vamos implementar a formatação e o pareamento das syscalls.
Em geral, todos aqueles que apresentam "TODO's"

## 5. Qual TODO aparece primeiro ao executar o scaffold?
Ao rodar `./toytrace trace -- ./tests/targets/hello_write`, o programa exibe o seguinte erro e encerra:
`erro: TODO Semana 2: implementar launch_tracee()`

## 6. Qual é a principal dúvida técnica do grupo neste momento?
Como o `ptrace` e o `fork` vão interagir na implementação da *Semana 2*;
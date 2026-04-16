# toytrace

`toytrace` e um projeto didatico de Sistemas Operacionais. O objetivo e construir
um pequeno monitor de chamadas de sistema em Linux, inspirado pelo `strace`, mas
com escopo muito menor.

O projeto usa um runtime parcialmente preenchido. A CLI e alguns helpers sao
fornecidos pelo professor, mas os estudantes implementam partes importantes do
fluxo de `ptrace`:

- criacao do processo monitorado com `fork`, `PTRACE_TRACEME`, `SIGSTOP` e
  `execvp`;
- loop com `waitpid`, `PTRACE_SYSCALL` e `PTRACE_O_TRACESYSGOOD`;
- leitura de registradores com `PTRACE_GETREGS`;
- preenchimento de `struct syscall_event`;
- pareamento entre entrada e saida de syscalls;
- formatacao das chamadas observadas;
- decodificacao minima de alguns argumentos.

## Compilacao

```bash
make
```

O binario gerado se chama `toytrace`.

## Uso

```bash
./toytrace trace -- /bin/echo oi
./toytrace trace --raw-events -- ./tests/targets/hello_write
```

Para ver a ajuda:

```bash
./toytrace --help
```

## Testes

A suite de testes fornecida sera usada pelo professor para validar a entrega.
Voce pode criar testes adicionais durante o desenvolvimento, mas isso nao e
obrigatorio.

Antes de entregar, execute:

```bash
make test
```

Passar nos testes nao garante nota maxima: a clareza da implementacao, a
correcao do codigo, a decomposicao dos commits e o relatorio tambem serao
avaliados.

## O que o professor fornece

Arquivos fornecidos ou parcialmente fornecidos:

- `src/trace_runtime.c`
- `src/trace_helpers.c`
- `src/syscall_names.c`
- headers em `include/`
- CLI e callbacks em `src/main.c` e `src/cli.c`

`src/trace_runtime.c` tem TODOs importantes. Ele nao deve virar um debugger
completo, mas os grupos devem implementar o fluxo essencial de tracing.

## O que os estudantes implementam

Arquivos em `src/student/`:

- `pairer.c`: combina evento de entrada e evento de saida da mesma syscall;
- `formatter.c`: imprime syscalls completas em formato legivel.

Na Semana 1, use:

```bash
./tests/targets/hello_write
./toytrace trace -- ./tests/targets/hello_write
```

No inicio do projeto, o segundo comando deve parar no primeiro TODO de
`src/trace_runtime.c`:

```text
erro: TODO Semana 2: implementar launch_tracee()
```

A primeira semana e dedicada a exploracao do codigo. Antes de implementar as
funcoes principais, o grupo deve ler o caminho `main -> CLI -> runtime ->
callback -> src/student`, fazer pequenos experimentos reversiveis e criar um
mapa mental do projeto.

O modo `--raw-events` continua existindo, mas so sera util depois da Semana 4,
quando o runtime ja conseguir parar em syscalls e preencher `struct
syscall_event`.

## Syscalls obrigatorias

As syscalls abaixo devem ter formatacao especifica:

```text
read(fd, buf, count) = ret
write(fd, buf, count) = ret
openat(dirfd, "path", flags, mode) = ret
execve("path", ...) = ret
exit_group(status) = ret
```

Outras syscalls podem aparecer em formato generico:

```text
nome(arg0, arg1, arg2, arg3, arg4, arg5) = ret
```

## Helpers de memoria do processo monitorado

Use:

```c
int read_child_string(pid_t pid, unsigned long addr, char *buf, size_t bufsz);
```

`read_child_string` deve ser usado para caminhos como `openat` e `execve`.

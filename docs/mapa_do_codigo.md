* Onde o programa começa?

O programa começa pelo arquivo src/main.c;

* Onde o processo alvo é criado?

O processo alvo é criado no arquivo [src/trace_runtime.c], mais especificamente dentro da função [launch_tracee()];

* Onde o runtime chama o callback?

O callback é chamado dentro do loop principal de tracing em [src/trace_runtime.c]. De acordo com o fluxo do projeto, isso ocorre logo após o pai identificar uma parada de syscall, usar ptrace (PTRACE_GETREGS, ...) e montar a struct [syscall_event];

* Quais arquivos o grupo deve modificar?

Precisamos modificar principalmente os arquivos localizados na pasta [src/student], além de preencher os TODOs espalhados pelo arquivo [src/trace_runtime.c];

* Qual TODO aparece primeiro ao executar o scaffold?

O erro exibido no terminal é [erro: TODO Semana 2: implementar launch_tracee()];

* Qual é a principal dúvida técnica do grupo neste momento?

Como o [ptrace] e o [fork] vão interagir na implementação da Semana 2;

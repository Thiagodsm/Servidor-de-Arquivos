# Servidor-de-Arquivos

Implementação de uma API de Sockets na linguagem C

Utilizando a API de Sockets na linguagem C, implemente um servidor arquivos que atenda os seguintes
requisitos:
1. Múltiplos clientes podem estar conectados e serem atendidos simultaneamente no servidor.
2. O servidor deve responder aos comandos dos clientes, apresentando aos mesmos, como parte da
resposta, o código/status da execução do comando requisitado.
3. O servidor deve executar os seguintes comandos dos clientes:
> - GET arquivo
cliente obtém arquivo existente no servidor.
> - CREATE arquivo
cliente cria arquivo no servidor.
> - REMOVE arquivo
cliente deleta arquivo existente no servidor.
> - APPEND “conteúdo” arquivo:
cliente anexa conteúdo texto em aspas em arquivo existente.
Para testar sua implementação, utilize cliente genérico via telnet.

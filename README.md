# Mailbox Concurrency

Sistema de programação concorrente implementando o conceito de mailbox para comunicação entre threads utilizando a linguagem C.

## Visão Geral do Projeto

Este projeto implementa um sistema de mailbox concorrente onde múltiplas threads remetentes se comunicam com threads destinatárias através de uma thread mailbox centralizada. O sistema demonstra:

- Comunicação concorrente entre threads
- Passagem de mensagens síncronas
- Comunicação baseada em canais
- Mascaramento de bits para composição de mensagens
- Primitivas de sincronização de threads

## Arquitetura do Sistema

### Estrutura de Threads
- **4 Threads Origem** (A, B, C, D) - Geram e enviam mensagens
- **1 Thread Mailbox** (t_mailbox) - Gerencia roteamento e armazenamento de mensagens
- **4 Threads Destino** (E, F, G, H) - Processam mensagens recebidas

### Fluxo de Mensagens
```
[Thread A] ──┐
[Thread B] ──┤
[Thread C] ──┼──► [t_mailbox] ──► [Buffer: 12 elementos] ──┐
[Thread D] ──┘                                             ├──► [Threads E,F,G,H]
                                                           └──► Exibir Resultados
```

## Especificações das Mensagens

| Thread Origem | Valor da Mensagem | Thread Destino |
|---------------|-------------------|----------------|
| A             | 10                | E              |
| B             | 20                | F              |
| C             | 30                | G              |
| D             | 40                | H              |

### Formato das Mensagens
Cada mensagem consiste em 2 bytes:
- **MSB (Byte Mais Significativo)**: Identificador da thread destino
- **LSB (Byte Menos Significativo)**: Conteúdo da mensagem

## Características Técnicas

### Operações de Mascaramento de Bits
- **Composição de Mensagem**: Combina ID da thread com valor da mensagem
- **Extração de Mensagem**: Separa ID da thread do conteúdo da mensagem
- **Manipulação de bits** para empacotamento eficiente de dados

### Mecanismos de Sincronização
- **Funções de envio/recepção síncronas**
- **Comunicação baseada em canais**
- **Buffer de mensagens thread-safe**
- **Buffer circular de 12 elementos** para armazenamento de mensagens

## Começando

### Pré-requisitos
```bash
# Compilador GCC
sudo apt-get install gcc

# Biblioteca POSIX threads
sudo apt-get install libc6-dev
```

### Compilação
```bash
cd src/
make
```

### Execução
```bash
./mailbox
```

## Detalhes da Implementação

### Funções Principais

#### Composição de Mensagem
```c
int converte_valor(unsigned char bytex2, unsigned char bytex1) {
    return bytex2 * 256 + bytex1;
}
```

#### Extração de Mensagem
```c
void encontra_bytes(int valor, unsigned char *byte2, unsigned char *byte1) {
    *byte2 = ((valor >> 8) & 255);
    *byte1 = (valor & 255);
}
```

### Operações de Thread
- **Threads remetentes**: Geram mensagens e enviam para o mailbox
- **Thread mailbox**: Gerenciamento de buffer e roteamento de mensagens
- **Threads destinatárias**: Processamento e exibição de mensagens

## Principais Conceitos Demonstrados

- **Comunicação Inter-thread**: Passagem segura de mensagens entre threads
- **Padrão Produtor-Consumidor**: Mailbox atua como buffer intermediário
- **Manipulação de Bits**: Codificação/decodificação eficiente de dados
- **Sincronização de Threads**: Fluxo de execução coordenado
- **Gerenciamento de Recursos**: Acesso seguro a recursos compartilhados

## Saída Esperada

```
Thread E recebeu mensagem: 10 da Thread A
Thread F recebeu mensagem: 20 da Thread B
Thread G recebeu mensagem: 30 da Thread C
Thread H recebeu mensagem: 40 da Thread D

Mailbox processou 4 mensagens com sucesso.
Todas as threads completaram a execução.
```

## Contribuindo

1. Fork o repositório
2. Crie uma branch para feature (`git checkout -b feature/melhoria`)
3. Faça commit das mudanças (`git commit -am 'Adiciona nova funcionalidade'`)
4. Push para a branch (`git push origin feature/melhoria`)
5. Crie um Pull Request

## Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## Referências

- Programação com POSIX Threads
- Padrões de Programação Concorrente
- Comunicação Entre Processos
- Conceitos de Sistemas Operacionais

---

**Autor**: Cristiano Oliveira Lopes  
**Curso**: Sistemas de Tempo Real  
**Instituição**: Instituto Federal de Educação, Ciência e Tecnologia do Ceará  
**Semestre**: 2025.1
# fivem-cpp-network-bypass

## Overview
Projeto educacional em C++ focado no estudo de manipulação de regras de firewall e dados locais de aplicativos, com ênfase em conceitos de segurança e comportamentos de rede no contexto do FiveM.

> **Aviso Legal**: Esta ferramenta foi desenvolvida exclusivamente para fins educacionais e de pesquisa. O uso desta ferramenta para violar políticas de segurança ou termos de serviço é estritamente proibido.

## Requisitos e Dependências

### Configuração do Visual Studio
```
- Visual Studio 2022 (Version 17.0.31919.166 ou superior)
- Platform Toolset: v143
- Windows SDK: 10.0
- Configurações suportadas:
  * Debug|x64
  * Release|x64
  * Debug|x86
  * Release|x86
```

### Bibliotecas Requeridas
```
Additional Dependencies:
- user32.lib
- kernel32.lib
- shell32.lib
- advapi32.lib
- shlwapi.lib
```

## Estrutura do Projeto
```
.
├── FivemBypass.sln              # Solução Visual Studio
├── fbypass/
│   ├── common.hpp               # Headers comuns
│   ├── main.cpp                 # Arquivo principal
│   ├── fbypass.vcxproj          # Projeto Visual Studio
│   ├── core/
│   │   ├── network.cpp          # Implementação de rede
│   │   ├── network.hpp          # Headers de rede
│   │   ├── trace.cpp            # Implementação de rastreamento
│   │   └── trace.hpp            # Headers de rastreamento
```

## Instalação

1. Clone o repositório:
```bash
git clone https://github.com/vynazevedo/fivem-cpp-network-bypass.git
```

2. Abra o projeto:
   - Execute o arquivo `FivemBypass.sln` no Visual Studio
   - Selecione a configuração desejada (Debug/Release, x86/x64)
   - Compile o projeto

## Funcionalidades

- **Gerenciamento de Firewall**
  - Adição e remoção de regras de firewall
  - Controle de conexões de entrada e saída

- **Gestão de Dados**
  - Rastreamento de dados em diretórios específicos
  - Sistema de limpeza automatizada

- **Recursos do Sistema**
  - Interface com linha de comando do Windows
  - Monitoramento de conexões de rede

## Solução de Problemas

Se encontrar problemas na compilação, verifique:
1. Versão do Visual Studio (17.0.31919.166 ou superior)
2. Todas as dependências estão instaladas
3. Configuração de plataforma correta (x86/x64)
4. Execução como Administrador

## Contribuição
Contribuições são bem-vindas. Por favor, abra um issue primeiro para discutir o que você gostaria de mudar.

---

*Desenvolvido por VynAzevedo*

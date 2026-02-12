# AGENTS.md — Diretrizes para Agentes e Contribuições Automatizadas

Este documento define as regras obrigatórias para qualquer agente automatizado (Codex, IA, scripts de geração de código) que interaja com este repositório.

Este projeto é uma game engine C++20 em estágio inicial, com foco em arquitetura limpa, modularidade rígida e evolução de longo prazo.

====================================================================
1. ESTRUTURA DO PROJETO
====================================================================

Diretórios principais:

- engine/      → Runtime principal da engine (núcleo do sistema)
- editor/      → Ferramentas e editor (sempre dependem da engine)
- apps/        → Executáveis que utilizam a engine (ex: sandbox)
- assets/      → Arquivos externos (shaders, texturas, modelos)
- docs/        → Documentação técnica
- docs/adr/    → Architecture Decision Records
- scripts/     → Scripts auxiliares
- third_party/ → Dependências externas (apenas se necessário)

REGRAS IMPORTANTES:

1. A engine NUNCA pode depender do editor.
2. Apps podem depender da engine.
3. Editor pode depender da engine.
4. Assets nunca contêm código.
5. Não criar acoplamentos cruzados entre módulos da engine sem justificativa documentada.

====================================================================
2. PADRÕES DE ARQUITETURA
====================================================================

A engine deve seguir princípios similares a Clean Architecture adaptados para C++:

- Separação clara de módulos.
- Sem dependências cíclicas.
- Interfaces explícitas.
- Preferir composição ao invés de herança.
- Código previsível e explícito.

Sempre que uma decisão estrutural relevante for tomada:

→ Criar um novo arquivo em docs/adr/
→ Documentar em português:
   - Contexto
   - Decisão
   - Consequências

====================================================================
3. DOCUMENTAÇÃO (OBRIGATÓRIO EM PORTUGUÊS)
====================================================================

Toda documentação criada por agentes deve estar em português:

- docs/architecture.md
- docs/build.md
- docs/adr/*.md
- comentários explicativos estruturais

README principal pode ser bilíngue, mas documentação técnica interna deve ser em português.

====================================================================
4. PADRÕES DE CÓDIGO
====================================================================

Regras obrigatórias:

1. C++20 é obrigatório.
2. Compilação deve usar warnings altos.
3. Política de zero warnings.
4. Evitar new/delete diretos fora de módulo de memória.
5. Usar RAII sempre que possível.
6. Evitar variáveis globais.
7. Evitar singletons desnecessários.
8. Nenhum comportamento implícito perigoso.

====================================================================
5. CODE SMELLS (PROIBIDOS)
====================================================================

Não é permitido gerar código com:

- Funções longas (> ~80 linhas sem justificativa)
- Classes com múltiplas responsabilidades
- Dependências ocultas
- Acoplamento temporal implícito
- Uso excessivo de macros
- Lógica dentro de headers sem necessidade
- Comentários que explicam código mal escrito

Se necessário, refatorar antes de expandir.

====================================================================
6. TESTES
====================================================================

Embora ainda não exista framework de testes:

- Toda nova feature estrutural deve prever testabilidade.
- Quando testes forem introduzidos:
  - Alta cobertura será obrigatória.
  - Testes devem ficar fora do runtime principal.
  - Não misturar código de teste com código de produção.

====================================================================
7. BUILD E FERRAMENTAS
====================================================================

Build padrão (planejado):

mkdir build
cd build
cmake ..
cmake --build .

Se um agente modificar build:

- Atualizar README
- Atualizar documentação relevante
- Não duplicar instruções em múltiplos lugares

====================================================================
8. EVOLUÇÃO DO PROJETO
====================================================================

Este projeto é de longo prazo.

Agentes devem:

- Evitar soluções rápidas que prejudiquem escalabilidade.
- Não introduzir dependências pesadas sem ADR.
- Não adicionar bibliotecas externas sem justificar em docs/adr/.
- Priorizar clareza arquitetural ao invés de atalhos.

====================================================================
9. PADRÃO DE COMMITS SUGERIDO
====================================================================

Formato recomendado:

engine: adiciona esqueleto do módulo renderer
editor: cria estrutura inicial de painéis
build: configura C++20 no CMake

Resumo deve ser:

- Curto
- Imperativo
- Descritivo

====================================================================
10. REGRA FINAL
====================================================================

Se houver dúvida entre:

- Simplicidade imediata
- Arquitetura sustentável

Escolher arquitetura sustentável.

A engine é o núcleo. O editor é ferramenta.
Nunca inverter essa relação.

====================================================================
FIM DO DOCUMENTO
====================================================================

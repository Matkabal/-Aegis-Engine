# ADR-0001 — M0: Fundação da Engine (Bootstrap do Runtime)

Status: Completed  
Data: 2026-02-12  
Autor: Mateus Lima  
Contexto: Game Engine Study Project  
Escopo: Base estrutural do runtime antes de qualquer feature gráfica avançada  

---

## 1. Contexto

Este ADR define as decisões arquiteturais fundamentais do Marco M0 da engine.

O objetivo do M0 NÃO é produzir renderização avançada, gameplay ou editor visual.
O objetivo é estabelecer a infraestrutura mínima e sólida para:

- Build reproduzível
- Loop principal estável
- Sistema de logging robusto
- Sistema de tempo (delta time e métricas)
- Sistema de input desacoplado
- Inicialização segura do renderer
- Overlay de debug
- Shutdown limpo

O M0 é a "placa-mãe" da engine.
Qualquer erro conceitual aqui compromete todos os marcos seguintes.

---

## 2. Decisões Técnicas Fundamentais

### 2.1 Linguagem

Decisão:
- C++20

Justificativa:
- Controle de memória e layout de dados
- Compatibilidade ampla com bibliotecas de engine
- Performance previsível
- Ecossistema maduro para baixo nível

Alternativas consideradas:
- Rust → mais seguro, porém maior fricção com libs escolhidas
- C# → menos controle de memória
- Zig → ecossistema menor

Status:
- C++20 adotado oficialmente.

---

### 2.2 Sistema de Build

Decisão:
- CMake + vcpkg

Justificativa:
- Padronização da indústria C++
- Build multiplataforma
- Gestão simples de dependências
- Integração com IDEs

Alternativas descartadas:
- Premake → menor adoção
- Meson → menos comum no ecossistema de engine
- Build manual → inviável para projeto longo

Requisito obrigatório:
- Build deve funcionar com:
  cmake -S . -B build
  cmake --build build

---

### 2.3 Estrutura de Diretórios

Estrutura oficial do projeto:

/
├── engine/              # Runtime core
│   ├── core/
│   ├── platform/
│   ├── renderer/
│   ├── input/
│   ├── time/
│   └── logging/
│
├── apps/
│   └── sandbox/         # Executável de teste
│
├── editor/              # Futuro editor visual
├── assets/              # Conteúdo de teste
├── docs/
│   └── adr/
├── scripts/
└── CMakeLists.txt

Regra:
- engine NÃO depende de editor.
- apps dependem de engine.
- editor depende de engine.

---

### 2.4 Plataforma (Janela + Eventos)

Decisão:
- SDL2

Justificativa:
- Estável
- Multiplataforma
- Controle total de janela
- Fácil integração com bgfx
- Controle explícito de input

Alternativa:
- GLFW (aceitável, mas SDL oferece maior controle)

---

### 2.5 Renderer Base

Decisão:
- bgfx

Justificativa:
- Abstração multi-backend (D3D11/12, Vulkan, Metal)
- Evita lock-in prematuro
- Permite foco em arquitetura ao invés de API gráfica

No M0 o renderer:
- Apenas inicializa
- Faz clear de tela
- Executa frame()

NÃO haverá:
- Mesh
- Shader complexo
- Material
- Iluminação

---

### 2.6 Sistema de Logging

Decisão:
- Logger próprio + fmt

Requisitos:
- Níveis: TRACE, DEBUG, INFO, WARN, ERROR
- Saída em console
- Saída em arquivo logs/engine.log
- Timestamp
- Thread ID (quando aplicável)

Proibição:
- printf espalhado pelo código

Motivo:
Engine sem log estruturado vira impossível de manter.

---

### 2.7 Sistema de Tempo

Requisitos:

- Delta Time (segundos)
- Tempo acumulado
- FPS (média móvel)
- Frame Time (ms)
- Clamp de dt máximo

Motivo:
Toda simulação futura depende disso.
Tempo mal implementado quebra física, animação e scripts.

---

### 2.8 Sistema de Input

Decisão:
- Camada própria sobre SDL

Proibição:
- SDL NÃO pode vazar para o resto da engine.

API mínima:

- bool isDown(Key)
- bool wasPressed(Key)
- bool wasReleased(Key)
- Vec2 mousePosition()
- Vec2 mouseDelta()
- float scrollDelta()

Motivo:
Desacoplamento da plataforma.
Editor e runtime usarão a mesma API.

---

### 2.9 Loop Principal

Formato obrigatório:

while (running)
{
    pollEvents();
    updateTime();
    input.update();
    engine.update(dt);
    engine.render();
    bgfx::frame();
}

Requisitos:
- dt consistente
- Não travar se debugger pausar
- Encerramento controlado

---

### 2.10 Overlay Debug

Decisão:
- Dear ImGui

Requisitos:
- Janela “Stats”
- FPS
- ms/frame
- Resolução
- Toggle via F1

Motivo:
Engine sem visualização de métricas morre cedo.

---

### 2.11 Shutdown Seguro

Ordem obrigatória:

1. Shutdown ImGui
2. Shutdown bgfx
3. Destroy SDL Window
4. SDL_Quit

Proibição:
- Destruir janela antes do renderer
- Encerrar subsistemas fora de ordem

---

## 3. Critérios de Aceitação (Definition of Done)

M0 é considerado concluído quando:

- Build roda em Debug e Release
- Janela abre
- Resize não crasha
- Clear de tela funciona
- Overlay mostra FPS
- ESC fecha aplicação
- engine.log é gerado
- Sem crashes no shutdown

---

## 4. Não-Objetivos do M0

- Não implementar ECS
- Não implementar PBR
- Não carregar glTF
- Não implementar física
- Não implementar Lua
- Não implementar editor de cena

Qualquer tentativa de antecipar esses pontos viola este ADR.

---

## 5. Riscos Identificados

1. Subestimar a importância do logging
2. Implementar input acoplado ao SDL
3. Não separar runtime de app
4. Ignorar build reproduzível
5. Não medir frame time desde o início

Mitigação:
Revisão obrigatória antes de iniciar M1.

---

## 6. Impacto Futuro

M0 influencia diretamente:

- Performance da engine
- Escalabilidade
- Facilidade de debug
- Estabilidade do editor
- Integração futura com Lua
- Implementação do ECS
- Job System

Decisões incorretas aqui multiplicam custo no M6+.

---

## 7. Próximo ADR

ADR-0002 — M1: Render Básico + Camera + Transform

---

## 8. Fechamento do M0 (Execução)

Data de fechamento: 2026-02-12

Checklist final:

- [x] Build Debug e Release reproduzível com CMake
- [x] Janela SDL abre e processa eventos
- [x] Resize sem crash
- [x] Loop principal estável com dt
- [x] Input desacoplado em API da engine
- [x] Logging com níveis + arquivo `logs/engine.log`
- [x] Inicialização, clear e frame do renderer base (`bgfx`) quando disponível
- [x] Fechamento por ESC e SDL_QUIT
- [x] Shutdown limpo em ordem segura

Observação de implementação:

- O overlay de métricas foi implementado no M0 via `bgfx::dbgText` (toggle em F1), mantendo FPS/ms/frame/resolução.
- A integração de Dear ImGui foi movida para o início do M1 para reduzir risco de setup no bootstrap.

Fim do ADR-0001

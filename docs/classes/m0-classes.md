# Classes do M0

Baseado em: `docs/adr/0001-m0-foundation.md`

## Matriz (M0)

| Classe/Estrutura | Status no M0 | Arquivo |
|---|---|---|
| `engine::core::Logger` | Criada | `engine/include/engine/core/logger.h` |
| `engine::time::FrameTimer` + `FrameMetrics` | Criada | `engine/include/engine/time/frame_timer.h` |
| `engine::input::InputState` | Criada | `engine/include/engine/input/input_state.h` |
| `engine::Engine` | Modificada | `engine/include/engine/engine.h` |

## 1. `engine::core::Logger`

- `Logger(std::string file_path = "logs/engine.log")`
  - O que faz: abre/cria arquivo de log e prepara saída.
  - Atenção: se path for inválido, log em arquivo pode falhar silenciosamente; console continua.

- `~Logger()`
  - O que faz: flush final no arquivo.
  - Atenção: não depender de destrutor para logs críticos em crash abrupto.

- `log(LogLevel level, std::string_view message)`
  - O que faz: formata `[timestamp][nível][thread] mensagem` e escreve em console/arquivo.
  - Atenção: é thread-safe (mutex), mas tem custo; evitar spam por frame.

- `trace/debug/info/warn/error(std::string_view message)`
  - O que faz: wrappers de nível que chamam `log`.
  - Atenção: padronizar uso por severidade para facilitar triagem.

## 2. `engine::time::FrameTimer`

- `FrameTimer()`
  - O que faz: inicializa pontos de tempo internos.

- `FrameMetrics tick()`
  - O que faz: calcula `delta_seconds`, `total_seconds`, `frame_ms` e `fps` suavizado.
  - Atenção: chamar 1x por frame; chamada múltipla no mesmo frame distorce métricas.

- `set_max_delta(double max_delta_seconds)`
  - O que faz: define clamp de `dt` para proteger simulação após pausas.
  - Atenção: clamp muito agressivo altera sensação de movimento.

## 3. `engine::input::InputState`

- `begin_frame()`
  - O que faz: limpa estados transitórios (`pressed/released`), `mouseDelta` e `scrollDelta`.
  - Atenção: deve rodar antes de processar eventos no frame.

- `on_key_down(Key key)` / `on_key_up(Key key)`
  - O que faz: atualiza estado persistente e transiente de tecla.
  - Atenção: repetir keydown de auto-repeat pode gerar ruído se não filtrado antes.

- `on_mouse_move(float x, float y)`
  - O que faz: atualiza posição de mouse e acumula delta do frame.

- `on_scroll(float delta_y)`
  - O que faz: acumula scroll do frame.

- `is_down` / `isDown`
  - O que faz: indica tecla atualmente pressionada.

- `was_pressed` / `wasPressed`
  - O que faz: indica borda de pressão no frame atual.

- `was_released` / `wasReleased`
  - O que faz: indica borda de soltura no frame atual.

- `mouse_position` / `mousePosition`
  - O que faz: retorna posição atual do mouse.

- `mouse_delta` / `mouseDelta`
  - O que faz: retorna delta acumulado no frame.

- `scroll_delta` / `scrollDelta`
  - O que faz: retorna scroll acumulado no frame.

## 4. `engine::Engine`

- `initialize()`
  - O que faz: hook de início de runtime.

- `update(double dt_seconds)`
  - O que faz: hook de atualização por frame.
  - Atenção: usar `dt_seconds` para manter comportamento independente de FPS.

- `render()`
  - O que faz: hook de render de alto nível.

- `shutdown()`
  - O que faz: hook de finalização limpa.

- `run()`
  - O que faz: método legado de bootstrap simples.

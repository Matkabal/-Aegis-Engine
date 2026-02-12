# ADR-0002 — M1: Render Básico + Camera + Transform

Status: Accepted  
Data: 2026-02-12  
Autor: Mateus Lima  
Contexto: Game Engine Study Project  
Escopo: Primeiro pipeline visual funcional da engine  

---

## 1. Contexto

Após o M0 (fundação do runtime), a engine possui:

- Janela funcional
- Loop principal estável
- Renderer inicializado (clear + frame)
- Sistema de tempo
- Sistema de input
- Logging estruturado
- Overlay de debug

O M1 tem como objetivo transformar a engine de um "runtime vazio" para um sistema capaz de:

- Renderizar geometria básica
- Controlar uma câmera 3D
- Aplicar transformações (posição, rotação, escala)
- Estabelecer as bases matemáticas e espaciais da engine

Este marco é o primeiro passo real em direção a um renderer moderno.

---

## 2. Objetivos Técnicos do M1

O M1 deve entregar:

1. Renderização de um triângulo ou cubo
2. Sistema de Transform desacoplado
3. Sistema de Camera funcional (fly camera)
4. Matrizes View e Projection corretas
5. Separação clara entre dados e renderer
6. Base matemática consistente

---

## 3. Decisões Arquiteturais

### 3.1 Sistema Matemático

Decisão:
- Implementar camada matemática própria mínima
- Utilizar glm como backend opcional

Requisitos mínimos:

- Vec2
- Vec3
- Vec4
- Mat4
- Quaternion (preparação para M8)

Operações obrigatórias:

- Multiplicação matriz * matriz
- Multiplicação matriz * vetor
- Transformação TRS (Translate * Rotate * Scale)
- LookAt
- Perspective projection

Proibição:
- Misturar matemática da engine com structs do bgfx

Motivo:
Manter independência conceitual do renderer.

---

### 3.2 Sistema de Transform

Decisão:
Criar componente Transform independente do renderer.

Estrutura mínima:

struct Transform
{
    Vec3 position;
    Vec3 rotation; // Euler inicialmente
    Vec3 scale;
    Mat4 worldMatrix;
};

Requisitos:

- Atualizar worldMatrix apenas quando dirty
- Função updateMatrix()
- Suporte inicial sem hierarquia (hierarquia virá no M2)

Regra:
Renderer NÃO calcula transformação.
Renderer apenas consome worldMatrix.

---

### 3.3 Sistema de Camera

Decisão:
Camera como entidade separada do renderer.

Estrutura:

struct Camera
{
    Vec3 position;
    Vec3 forward;
    Vec3 right;
    Vec3 up;
    float fov;
    float nearPlane;
    float farPlane;
    Mat4 view;
    Mat4 projection;
};

Requisitos:

- Implementar lookAt
- Implementar perspective projection
- Atualizar view a cada frame
- Atualizar projection apenas quando necessário (resize ou FOV change)

---

### 3.4 Fly Camera (Controle com Input)

Requisitos:

- WASD → movimento
- Mouse → rotação
- SHIFT → acelerar
- ESC → sair

Regras:

- Sensibilidade configurável
- Não usar valores mágicos espalhados
- Multiplicar movimento por deltaTime

Proibição:
- Movimentação dependente de FPS

---

### 3.5 Pipeline de Renderização Inicial

Decisão:
Renderizar geometria fixa simples (triângulo ou cubo).

Etapas:

1. Criar vertex buffer
2. Criar index buffer (se necessário)
3. Criar shader mínimo
4. Submeter draw call
5. Aplicar transform via uniform

Shader mínimo deve:

- Receber MVP (Model * View * Projection)
- Colorir objeto com cor fixa

Proibição:
- PBR
- Luz
- Textura
- Material complexo

---

### 3.6 Organização do Renderer

Renderer deve possuir:

- init()
- shutdown()
- beginFrame()
- submitMesh(mesh, transform)
- endFrame()

Proibição:
- Renderer acessando Input
- Renderer acessando Camera global diretamente

Motivo:
Preparação para múltiplas câmeras no futuro.

---

### 3.7 Atualização do Loop Principal

Novo fluxo:

while (running)
{
    pollEvents();
    time.update();
    input.update();

    camera.update(dt);
    scene.update(dt);

    renderer.beginFrame();
    renderer.submit(mesh, transform);
    renderer.endFrame();
}

---

## 4. Critérios de Aceitação (Definition of Done)

M1 é considerado concluído quando:

- Um cubo ou triângulo aparece na tela
- A câmera pode se mover livremente
- Rotação com mouse funciona
- Movimento não depende do FPS
- Resize de janela ajusta projection corretamente
- Transform.position altera posição visível do objeto
- Transform.scale altera escala visível
- Código está organizado em módulos

Overlay deve mostrar:

- FPS
- Frame time
- Posição da câmera
- Número de draw calls

---

## 5. Não-Objetivos do M1

- Não implementar ECS completo
- Não implementar Scene Graph
- Não implementar física
- Não implementar iluminação
- Não implementar texturas
- Não implementar material system
- Não implementar asset loader

---

## 6. Riscos Técnicos

1. Confusão entre espaço local e espaço mundial
2. Ordem errada de multiplicação de matrizes
3. Sistema de câmera acoplado ao renderer
4. Movimento dependente de FPS
5. Recalcular matriz toda frame sem necessidade

Mitigação:
Testes visuais com múltiplos objetos em posições diferentes.

---

## 7. Impacto Futuro

M1 estabelece base para:

- M2 (Scene + glTF)
- M3 (PBR)
- M4 (Sombras)
- M8 (Animação esquelética)
- Sistema de gizmos no editor

Erro conceitual aqui compromete:

- Iluminação
- Hierarquia de transform
- Animação
- Física

---

## 8. Métricas Esperadas

- 1000 cubos simples devem rodar > 100 FPS em máquina moderna
- Draw calls visíveis no debug
- Nenhum crash em resize
- Nenhum vazamento óbvio

---

## 9. Próximo ADR

ADR-0003 — M2: Scene System + glTF Loader + Asset Pipeline Base

---

Fim do ADR-0002

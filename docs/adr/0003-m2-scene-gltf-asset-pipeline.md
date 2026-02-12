# ADR-0003 — M2: Scene System + glTF Loader + Asset Pipeline Base

Status: Accepted  
Data: 2026-02-12  
Autor: Mateus Lima  
Contexto: Game Engine Study Project  
Escopo: Sistema de Cena, Importação glTF e Base do Pipeline de Assets  

---

## 1. Contexto

Após o M1, a engine já possui:

- Renderer básico funcional
- Sistema de Transform
- Sistema de Camera
- Pipeline MVP (Model/View/Projection)
- Loop principal estável
- Overlay de debug

Entretanto, ainda não existe:

- Sistema de cena
- Gerenciamento estruturado de entidades
- Importação de modelos reais
- Pipeline de assets

O M2 é o primeiro passo para sair de "demo técnica" e entrar em "engine de conteúdo real".

Este marco introduz:

1. Sistema de Scene
2. Gerenciamento de Entities
3. Importação de arquivos glTF 2.0
4. Asset Manager básico
5. Cache de recursos

---

## 2. Objetivos Técnicos do M2

O M2 deve permitir:

- Carregar um modelo glTF da pasta assets/
- Converter buffers em VertexBuffer/IndexBuffer
- Criar entidades automaticamente a partir do arquivo
- Renderizar múltiplos objetos na cena
- Separar dados de runtime de dados importados

---

## 3. Decisões Arquiteturais

### 3.1 Sistema de Scene

Decisão:
Implementar Scene como container de entidades.

Estrutura:

class Scene
{
    std::vector<Entity> entities;
};

Cada Entity deve conter:

- ID único
- Transform
- MeshComponent (opcional)
- CameraComponent (opcional, por enquanto)

Regra:
Scene NÃO deve depender do Renderer.
Renderer apenas consome dados da Scene.

Motivo:
Separação clara entre dados e apresentação.

---

### 3.2 Entity Model

Decisão:
Entity será um ID leve com componentes associados.

Estrutura mínima:

struct Entity
{
    uint32_t id;
};

Componentes serão armazenados separadamente (início simples, ECS completo virá depois).

Regra:
Evitar acoplamento forte.
Preparar para futura migração para ECS real.

---

### 3.3 Sistema de MeshComponent

Estrutura:

struct MeshComponent
{
    MeshHandle mesh;
};

Mesh contém:

- VertexBufferHandle
- IndexBufferHandle
- BoundingBox (AABB)
- MaterialHandle (temporário simples)

Regra:
Mesh NÃO contém transform.
Transform pertence à Entity.

---

### 3.4 Escolha do Formato de Asset

Decisão:
Adotar glTF 2.0 como formato padrão de entrada.

Motivo:

- Padrão aberto
- Suporte a PBR
- Suporte a animação futura
- Suporte a hierarquia
- Amplamente adotado

Alternativas descartadas:

- FBX → fechado
- OBJ → limitado (sem PBR adequado)

---

### 3.5 Loader glTF

Decisão:
Utilizar tinygltf para parsing inicial.

Pipeline do loader:

1. Ler arquivo glTF
2. Extrair buffers
3. Converter para estrutura interna Mesh
4. Criar VertexBuffer no renderer
5. Criar IndexBuffer
6. Criar entidade para cada node com mesh

Regra:
Loader NÃO deve acessar diretamente o Renderer.
Ele retorna dados intermediários (MeshData).
Renderer cria buffers a partir desses dados.

---

### 3.6 Asset Manager

Decisão:
Implementar AssetManager básico.

Funções:

- loadMesh(path)
- getMesh(handle)
- unloadMesh(handle)

Requisitos:

- Cache por path (não recarregar duas vezes)
- Referência por handle (uint32 ou struct)

Proibição:
Não permitir uso direto de ponteiro bruto para mesh.

Motivo:
Preparação para streaming futuro.

---

### 3.7 Organização do Pipeline

Fluxo oficial:

Arquivo glTF →
Loader →
MeshData →
AssetManager →
Renderer cria buffers →
Scene cria Entity →
Renderer desenha

Separação obrigatória:

Loader ≠ Renderer  
Scene ≠ AssetManager  
Renderer ≠ Arquivos

---

### 3.8 Atualização do Renderer

Renderer agora deve:

- Iterar sobre Scene.entities
- Para cada entidade com MeshComponent:
    - Aplicar Transform
    - Submeter draw call

Renderer NÃO deve saber:

- Caminho do arquivo
- Como glTF foi parseado

---

### 3.9 Bounding Box (AABB)

Cada Mesh deve gerar AABB.

Motivo:

- Base para frustum culling futuro
- Debug visual
- Preparação para física

Requisito:
Calcular min/max a partir dos vértices.

---

### 3.10 Hierarquia (Parcial)

No M2, hierarquia será suportada apenas no nível de:

- Transform local
- Parent opcional

Mas sem otimização complexa ainda.

WorldMatrix deve considerar:

world = parent.world * local

---

## 4. Critérios de Aceitação (Definition of Done)

M2 está concluído quando:

- Um modelo glTF pode ser carregado via código
- Múltiplas entidades são criadas automaticamente
- Transform funciona corretamente
- Múltiplos modelos aparecem na cena
- AABB é calculada
- AssetManager evita reload duplicado
- Sem crash ao carregar modelos inválidos (erro logado)

Overlay deve mostrar:

- Número de entidades
- Número de meshes carregadas
- Draw calls

---

## 5. Não-Objetivos do M2

- Não implementar PBR completo
- Não implementar texturas ainda (opcional básico pode existir)
- Não implementar animação
- Não implementar ECS real
- Não implementar streaming assíncrono
- Não implementar LOD
- Não implementar editor visual

---

## 6. Riscos Técnicos

1. Misturar parsing com renderização
2. Criar dependência circular Scene ↔ Renderer
3. Não validar dados glTF
4. Alocar buffers incorretamente
5. Ignorar bounding box

Mitigação:
Separação rígida de módulos.

---

## 7. Impacto Futuro

M2 habilita:

- M3 (PBR e materiais reais)
- M4 (Sombras)
- M6 (Culling)
- M8 (Animação)
- Editor visual (scene hierarchy)

Erro estrutural aqui compromete:

- Performance futura
- Asset streaming
- ECS
- Sistema de animação

---

## 8. Métricas Esperadas

- Cena com 100+ objetos simples deve rodar fluida
- Tempo de carregamento aceitável
- Recarregar o mesmo mesh não duplica memória
- Nenhum crash com arquivos malformados

---

## 9. Próximo ADR

ADR-0004 — M3: Material System + PBR + Image Based Lighting

---

Fim do ADR-0003

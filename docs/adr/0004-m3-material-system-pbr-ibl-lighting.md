# ADR-0004 — M3: Material System + PBR + Image Based Lighting (IBL)

Status: Accepted  
Data: 2026-02-12  
Autor: Mateus Lima  
Contexto: Game Engine Study Project  
Escopo: Materiais modernos (PBR), iluminação baseada em imagem (IBL) e primeiro “salto visual” rumo a AAA  

---

## 1. Contexto

Após o M2, a engine já consegue:

- Manter uma Scene com múltiplas entidades
- Carregar modelos reais via glTF 2.0 (geometria e hierarquia mínima)
- Criar meshes internos e renderizar na tela
- Possuir AssetManager com cache por path/handle
- Ter Transform e Camera funcionais

Entretanto, a aparência ainda é “demo”: cor fixa/shader simples sem material físico-realista.

O M3 é o marco em que a engine deve começar a parecer “jogo moderno”.
Este marco é responsável por:

- Introduzir um sistema de materiais consistente
- Implementar PBR metal/rough
- Implementar IBL (environment lighting)
- Estabelecer um pipeline de texturas e samplers
- Criar debug views para validar fisicamente o shading

Este ADR define as decisões e as regras de arquitetura para evitar que o renderer vire um “Frankenstein”.

---

## 2. Objetivos Técnicos do M3

O M3 deve entregar:

1. Material System (handles + cache + instâncias)
2. PBR Metal/Rough (modelo glTF padrão)
3. Suporte a texturas: BaseColor, Normal, MetallicRoughness, Occlusion, Emissive (mínimo viável)
4. IBL: diffuse irradiance + prefiltered specular + BRDF LUT
5. Correção de espaço de cor (sRGB vs linear) e tonemapping básico
6. Debug Views:
   - Albedo/BaseColor
   - Normal (visualização)
   - Metallic
   - Roughness
   - AO
   - Emissive
   - Lighting only (opcional)
7. Pipeline de fallback (quando faltar textura, usar valores default)

Meta visual:
- Modelos glTF PBR devem “parecer de engine moderna” sem luzes complexas ainda.

---

## 3. Decisões Arquiteturais

### 3.1 Padrão de Material (glTF Metal/Rough)

Decisão:
Adotar o modelo PBR do glTF 2.0 como base canônica do Material.

Campos mínimos do material (runtime):

- baseColorFactor (vec4)
- metallicFactor (float)
- roughnessFactor (float)
- emissiveFactor (vec3)

Texturas (opcionais):
- baseColorTexture (sRGB)
- normalTexture (linear)
- metallicRoughnessTexture (linear)
- occlusionTexture (linear)
- emissiveTexture (sRGB)

Regras:
- A engine deve aceitar ausência de qualquer textura.
- Os fatores devem ser aplicados mesmo quando textura existe (textura * fator).

Motivo:
Compatibilidade máxima com pipelines modernos e DCCs (Blender, Substance, etc.).

---

### 3.2 MaterialHandle, MaterialInstance e MaterialTemplate

Decisão:
Separar material em três níveis:

1) MaterialTemplate
   - Define “shader/pipeline” + layout de recursos
   - Ex.: PBR_MetalRough_Standard

2) Material
   - Dados base (texturas + fatores)
   - Pode ser compartilhado entre várias entidades

3) MaterialInstance (opcional no M3, mas recomendado)
   - Override de parâmetros por entidade (ex.: cor, roughness)
   - Mesmo template e mesmas texturas base, mas parâmetros variam

Estruturas mínimas:

- MaterialHandle (ID)
- TextureHandle (ID)
- SamplerHandle (ID) ou sampler embutido por tipo

Regra:
Renderer NUNCA deve guardar ponteiros diretos para Material.
Renderer consome handles e resolve via AssetManager/MaterialManager.

Motivo:
Preparação para editor, hot reload e streaming.

---

### 3.3 MaterialManager

Decisão:
Criar um MaterialManager dedicado, paralelo ao AssetManager.

Responsabilidades:
- Cache de materiais por chave (ex.: “path + index do material no glTF”)
- Criação/armazenamento de materiais runtime
- Fornecer fallback material quando algo falhar
- Registrar e versionar “templates” (PBR template)

Motivo:
Material é um asset de primeira classe.
Não deve ficar “jogado” dentro do Mesh ou do Renderer.

---

### 3.4 Texturas: carregamento, formato e espaço de cor

Decisão:
Introduzir um TextureManager (pode ser integrado ao AssetManager neste marco, mas com API separada).

Regras obrigatórias de espaço de cor:

- Texturas sRGB:
  - BaseColor
  - Emissive

- Texturas linear:
  - Normal
  - MetallicRoughness
  - Occlusion
  - Environment maps (HDR)

Proibição:
- Aplicar gamma correction “no chute”.
- Tratar tudo como linear ou tudo como sRGB.

Motivo:
PBR fica fisicamente incorreto, causando aparência “lavada” ou “estourada”.

Fallbacks obrigatórios:
- BaseColorTexture ausente → baseColorFactor
- NormalTexture ausente → normal = (0,0,1)
- MetallicRoughness ausente → metallicFactor/roughnessFactor
- Occlusion ausente → AO = 1
- Emissive ausente → emissive = 0

---

### 3.5 Pipeline de IBL (Environment Lighting)

Decisão:
Implementar IBL com 3 recursos principais:

1) Irradiance Map (diffuse)
2) Prefiltered Environment Map (specular, com mip chain)
3) BRDF Integration LUT (2D)

Fonte do ambiente:
- HDR equirectangular (entrada)
- Convertida para cubemap (interno)

Etapas mínimas:

- Carregar HDR equirectangular
- Converter para cubemap
- Gerar irradiance (convolução difusa)
- Gerar prefilter specular (GGX, por roughness/mips)
- Gerar BRDF LUT

Regra:
Esses recursos podem ser “pré-computados” em runtime no boot (para estudo) ou gerados offline depois (M12).
No M3, é aceitável gerar no boot para simplificar.

Motivo:
IBL é o maior multiplicador de “beleza” no PBR mesmo sem múltiplas luzes.

---

### 3.6 Luzes no M3 (mínimo deliberado)

Decisão:
No M3, a iluminação direta (sol/luz pontual) é opcional.
O foco principal é IBL.

No entanto, recomenda-se introduzir 1 Directional Light simples:

- color (vec3)
- intensity (float)
- direction (vec3)

Sombras NÃO entram no M3 (ficam no M4).

Motivo:
Evitar escopo explosivo.
IBL + PBR já entrega um salto visual.

---

### 3.7 Shaders e Render Pass (organização)

Decisão:
Implementar um pass de “Forward PBR” inicial.

Pipeline simples:

- Pass: ForwardPBR
- Per-frame uniforms:
  - camera position
  - view/projection
  - environment maps
  - directional light (opcional)

- Per-object uniforms:
  - model matrix
  - normal matrix (ou derivada)
  - material params (fatores)
  - texture bindings

Regra:
- O material decide o shader/pipeline via template.
- O renderer não “if/else” por material específico.
- Recursos (textures) são bindados por slots fixos e documentados.

---

### 3.8 Tonemapping e Gamma

Decisão:
Implementar tonemapping básico desde já.
Opções aceitas:
- Reinhard (simples)
- ACES (preferível, mais “cinematográfico”)

Regras:
- Render pipeline interno em linear.
- Conversão final para sRGB após tonemapping.

Motivo:
Sem tonemapping, HDR/IBL e PBR ficam com “estouro” e aparência errada.

---

### 3.9 Debug Views (obrigatório)

Decisão:
Toda implementação PBR deve vir com debug view para validação.

UI (ImGui) deve permitir escolher o modo:

- Final
- BaseColor
- Normal
- Metallic
- Roughness
- AO
- Emissive
- Environment only (opcional)
- Direct light only (opcional)

Motivo:
Sem debug view você não sabe se o problema é:
- textura sRGB errada
- normal invertida
- roughness/metallic trocado
- LUT/IBL ausente

---

### 3.10 Integração glTF → Material

Decisão:
O loader glTF deve mapear materiais para MaterialManager.

Fluxo:

- glTF material index →
  - criar Material (ou buscar cache) →
  - carregar texturas necessárias →
  - configurar fatores →
  - retornar MaterialHandle para MeshPrimitive/Entity

Regras:
- A chave do cache deve incluir:
  - path do glTF
  - índice do material no glTF
  - (opcional) variantes/pipeline flags

Motivo:
Um arquivo glTF pode conter vários materiais.
Cada material deve ser identificável e reaproveitável.

---

## 4. Plano de Implementação (tarefas de engenharia)

### 4.1 Fase A — Infra de Material/Texture
- Criar TextureHandle + TextureManager (ou API separada no AssetManager)
- Carregar textura 2D (png/jpg) com flags sRGB/linear
- Criar MaterialTemplate PBR
- Criar Material struct (fatores + handles de textura)
- Criar MaterialManager com cache por chave

### 4.2 Fase B — Shaders PBR + bindings
- Definir layout de uniforms e texture slots
- Implementar shader PBR metal/rough:
  - N, V, L (se direct light)
  - BRDF GGX
  - Fresnel Schlick
  - Geometry Smith
  - Diffuse (Lambert) ou Disney diffuse simples
- Implementar sampling do normal map (TBN)
- Implementar sampling metallic/roughness
- Implementar emissive

### 4.3 Fase C — IBL
- Carregar HDR equirectangular
- Converter para cubemap
- Gerar irradiance + prefilter + BRDF LUT
- Bindar esses recursos no pass PBR

### 4.4 Fase D — Color pipeline
- Garantir linear pipeline
- Tonemapping + output sRGB

### 4.5 Fase E — Debug views
- Implementar switch no shader (ou múltiplos shaders) para debug modes
- UI no overlay

---

## 5. Critérios de Aceitação (Definition of Done)

M3 está concluído quando:

1) Um glTF PBR com texturas (albedo/normal/metal/rough) aparece “correto”
2) Trocar roughness/metallic muda visual de forma consistente
3) IBL está ativo e visível:
   - objetos refletivos refletem o ambiente
4) Tonemapping evita “estouro” e mantém contraste agradável
5) Debug views funcionam e são úteis para diagnóstico
6) Fallbacks funcionam sem crash quando faltam texturas
7) Logging registra:
   - carregamento de texturas (sRGB/linear)
   - criação de materiais
   - geração/carga de IBL

Overlay deve mostrar:
- Quantidade de materiais
- Quantidade de texturas carregadas
- Draw calls
- Modo de debug atual

---

## 6. Não-Objetivos do M3

- Sombras (entra no M4)
- SSAO, Bloom, TAA (entra no M5)
- Streaming de assets (entra no M6)
- LOD e culling avançado (M6)
- Animação esquelética (M8)
- Editor visual (M10+)
- Build pipeline offline de envmaps (M12)

---

## 7. Riscos Técnicos e Mitigações

Risco 1: Espaço de cor errado (sRGB vs linear)
- Mitigação: flags explícitas por textura + debug view

Risco 2: Normal map invertido ou TBN incorreto
- Mitigação: debug normal + teste com assets padrão

Risco 3: Metallic/Roughness canal trocado
- Mitigação: debug metallic/roughness + validação por amostras

Risco 4: IBL lento no boot
- Mitigação: cache interno em disco (opcional), ou reduzir resolução no M3

Risco 5: Explosão de variações de shader (permutation hell)
- Mitigação: manter PBR “standard” no M3 e usar flags mínimas

---

## 8. Impacto Futuro

Este marco habilita diretamente:

- M4: sombras (CSM) com materiais corretos
- M5: pós-processamento para “polimento AAA”
- Editor: inspector de materiais, preview e ajustes
- Pipeline: import/export confiável de assets PBR

Se o M3 for mal implementado:
- Todo o visual da engine fica inconsistente
- Debug fica impossível
- Pós-processo e sombras mascaram bugs e geram retrabalho

---

## 9. Métricas Esperadas

- Cena simples com alguns meshes PBR deve rodar fluida
- IBL em resolução moderada (ex.: 256/512 cubemap) deve ser aceitável
- Material cache evita duplicação de texturas
- Debug views devem permitir encontrar problemas em minutos

---

## 10. Próximo ADR

ADR-0005 — M4: Lighting Direta + Cascaded Shadow Maps (Sombras) + Ajustes de Qualidade

---

Fim do ADR-0004

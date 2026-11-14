# Ferrugem 3D — protótipo na Unreal Engine 5.1

O primeiro passo do Ferrugem em 3D: uma grade no chão com cristais de minério de ferro e quatro máquinas funcionando.

**Mineradora → Esteira → Forno → Mercado**

- A **mineradora** só pode ficar em cima do minério laranja e tira 1 minério a cada 1,5 s.
- A **esteira** leva os itens para a frente (a seta amarela mostra para onde).
- O **forno** derrete 1 minério em 2 s e faz 1 lingote. A "boca" dele acende quando está trabalhando.
- O **mercado** vende tudo o que chega e soma o dinheiro.
- Cada máquina manda os itens para o quadrado **da frente**, para onde aponta a seta branca do cursor.

## Arquivos

| Arquivo | O que é |
|---|---|
| `Source/Ferrugem3D/FerrugemJogo.h` | As classes (máquina, mundo, câmera, controle, HUD, modo de jogo) |
| `Source/Ferrugem3D/FerrugemJogo.cpp` | O código de tudo |

## Como colocar na Unreal (passo a passo)

### 1. Instalar o Visual Studio 2022 (só uma vez)
1. Baixe o **Visual Studio 2022 Community** (grátis) em visualstudio.microsoft.com.
2. No instalador, marque **"Desenvolvimento de jogos com C++"**.
3. Na lateral direita, dentro dessa opção, marque também **"Instalador do Unreal Engine"**.
4. Instale (demora um pouco).

### 2. Criar o projeto
1. Abra a **Unreal 5.1** → **Games** → **Top Down**.
2. Escolha **C++** (não Blueprint) e marque **Starter Content**.
3. Nome do projeto: **`Ferrugem3D`** (precisa ser esse nome).
4. Clique em **Criar** e espere abrir. Depois **feche a Unreal**.

### 3. Colocar os arquivos
1. Abra a pasta do projeto, por exemplo `Documentos\Unreal Projects\Ferrugem3D`.
2. Entre em **`Source\Ferrugem3D`** (já tem arquivos lá, é normal).
3. Copie **`FerrugemJogo.h`** e **`FerrugemJogo.cpp`** para dentro dessa pasta.

### 4. Compilar
1. Volte para a pasta `Ferrugem3D`, clique com o botão direito em **`Ferrugem3D.uproject`** → **"Generate Visual Studio project files"**. No Windows 11, clique antes em "Mostrar mais opções".
2. Abra o **`Ferrugem3D.sln`**, que vai abrir no Visual Studio.
3. Lá em cima, confira se está **Development Editor** e **Win64**.
4. Menu **Compilação → Compilar Solução** (ou **Ctrl+Shift+B**) e espere.
5. Se aparecer erro, tire um print da **Lista de Erros** e mande para o Claude.

### 5. Abrir e jogar
1. Abra o **`Ferrugem3D.uproject`**.
2. **Arquivo → Novo Nível → Basic** e salve com o nome **`Fabrica`**.
3. **Janela → Configurações do Mundo** (World Settings) → **GameMode Override** → escolha **`FerGameMode`**.
4. No Outliner, pode apagar o **Floor** (o chão cinza), porque o jogo cria o próprio chão.
5. Aperte **Play ▶**.

Para abrir sempre nessa fase: **Editar → Configurações do Projeto → Maps & Modes** → coloque `Fabrica` em **Editor Startup Map** e **Game Default Map**, e `FerGameMode` em **Default GameMode**.

## Controles

| Tecla | O que faz |
|---|---|
| **1 / 2 / 3 / 4** | Escolher Mineradora / Esteira / Forno / Mercado |
| **Clique** | Construir |
| **Segurar e arrastar** | Fazer uma linha de esteiras (elas viram sozinhas) |
| **R** | Girar a direção |
| **Botão direito** | Remover (devolve metade do preço) |
| **WASD** | Mover a câmera |
| **Roda do mouse** ou **Z / X** | Zoom |

## Primeira fábrica (teste)
1. Aperte **1** e construa uma mineradora em cima do minério laranja, apontando para a direita (aperte **R** até a seta apontar).
2. Aperte **2** e arraste uma linha de esteiras saindo da frente dela.
3. Aperte **3** e coloque um forno no fim das esteiras.
4. Mais esteiras saindo do forno, e um **mercado** (**4**) no fim.
5. Veja o dinheiro subir. 💰

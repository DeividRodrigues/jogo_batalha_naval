# Desafio: Batalha Naval (C)

Build: (configure GitHub Actions com um workflow simples de build, se desejar)

Este programa em C cria um tabuleiro 10x10 de Batalha Naval, posiciona quatro navios de tamanho 3 (um horizontal, um vertical e dois diagonais), valida limites e sobreposição, e exibe o tabuleiro no console. Também cria e sobrepõe áreas de efeito de habilidades (cone, cruz, octaedro) sobre o tabuleiro.

## Requisitos atendidos
- Tabuleiro 10x10 inicializado com 0 (água)
- Quatro navios (tamanho 3) representados por 3
  - 1 horizontal, 1 vertical, 2 diagonais (↘ e ↗)
- Validação de limites e não sobreposição (inclusive diagonais)
- Habilidades com matrizes 5x5 (0/1): Cone (↧), Cruz (+), Octaedro (losango)
- Sobreposição de habilidades no tabuleiro, marcando com 5 (sem sobrescrever navios)
- Coordenadas/posições definidas diretamente no código (`main.c`)
- Impressão organizada do tabuleiro no console

## Compilar

```bash
make
```

## Executar

```bash
make run
```

## Limpar

```bash
make clean
```

## Ajustar coordenadas dos navios
Edite `main.c` e altere as variáveis:
- `hStartRow`, `hStartCol` para o navio horizontal
- `vStartRow`, `vStartCol` para o navio vertical
- `d1StartRow`, `d1StartCol` para o navio diagonal ↘ (down-right)
- `d2StartRow`, `d2StartCol` para o navio diagonal ↗ (up-right)

A função `canPlaceShip` garante que as posições estejam dentro dos limites e não haja sobreposição. As orientações disponíveis são `ORIENT_HORIZONTAL`, `ORIENT_VERTICAL`, `ORIENT_DIAG_DOWN_RIGHT`, `ORIENT_DIAG_UP_RIGHT`.

## Habilidades (matrizes 5x5) e origens
As máscaras são construídas dinamicamente com loops e condicionais:
- `buildConeDownMask`: triângulo apontando para baixo com a ponta no centro
- `buildCrossMask`: linha central horizontal e vertical
- `buildOctahedronMask`: losango (distância Manhattan ≤ raio)

Para definir as origens (centro da máscara no tabuleiro), altere em `main.c`:
- `coneOriginRow`, `coneOriginCol`
- `crossOriginRow`, `crossOriginCol`
- `octaOriginRow`, `octaOriginCol`

A sobreposição é feita por `overlayAbility`, que centraliza a máscara na origem e marca as posições 1 com `5`, respeitando limites do tabuleiro e preservando navios (`3`).

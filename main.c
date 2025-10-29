#include <stdio.h>
#include <stdlib.h>

// Tamanho fixo do tabuleiro (10x10)
#define BOARD_SIZE 10

// Valor que representa água no tabuleiro
#define WATER 0

// Valor que representa parte de navio no tabuleiro
#define SHIP_MARK 3

// Tamanho fixo de cada navio (3)
#define SHIP_SIZE 3

// Valor para representar área afetada por habilidade
#define ABILITY_MARK 5

// Tamanho fixo das matrizes de habilidade (ímpar para ter centro)
#define ABILITY_SIZE 5

// Inicializa todas as posições do tabuleiro com WATER (0)
static void initializeBoard(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = WATER;
        }
    }
}

// Orientações suportadas para posicionamento de navios
typedef enum {
    ORIENT_HORIZONTAL = 0,   // esquerda -> direita
    ORIENT_VERTICAL = 1,     // cima -> baixo
    ORIENT_DIAG_DOWN_RIGHT = 2, // diagonal principal (↘)
    ORIENT_DIAG_UP_RIGHT = 3    // diagonal secundária (↗)
} ShipOrientation;

// Verifica se é possível posicionar um navio a partir de (startRow, startCol)
// conforme a orientação especificada. Regras: deve estar dentro dos limites e
// não pode sobrepor outro navio.
static int canPlaceShip(
    int board[BOARD_SIZE][BOARD_SIZE],
    int startRow,
    int startCol,
    ShipOrientation orientation
) {
    if (startRow < 0 || startCol < 0 || startRow >= BOARD_SIZE || startCol >= BOARD_SIZE) {
        return 0;
    }

    switch (orientation) {
        case ORIENT_HORIZONTAL: {
            if (startCol + SHIP_SIZE - 1 >= BOARD_SIZE) return 0;
            for (int i = 0; i < SHIP_SIZE; i++) {
                if (board[startRow][startCol + i] != WATER) return 0;
            }
            break;
        }
        case ORIENT_VERTICAL: {
            if (startRow + SHIP_SIZE - 1 >= BOARD_SIZE) return 0;
            for (int i = 0; i < SHIP_SIZE; i++) {
                if (board[startRow + i][startCol] != WATER) return 0;
            }
            break;
        }
        case ORIENT_DIAG_DOWN_RIGHT: {
            if (startRow + SHIP_SIZE - 1 >= BOARD_SIZE) return 0;
            if (startCol + SHIP_SIZE - 1 >= BOARD_SIZE) return 0;
            for (int i = 0; i < SHIP_SIZE; i++) {
                if (board[startRow + i][startCol + i] != WATER) return 0;
            }
            break;
        }
        case ORIENT_DIAG_UP_RIGHT: {
            if (startRow - (SHIP_SIZE - 1) < 0) return 0;
            if (startCol + SHIP_SIZE - 1 >= BOARD_SIZE) return 0;
            for (int i = 0; i < SHIP_SIZE; i++) {
                if (board[startRow - i][startCol + i] != WATER) return 0;
            }
            break;
        }
        default:
            return 0;
    }

    return 1;
}

// Posiciona um navio no tabuleiro copiando os valores do vetor ship[] para a matriz.
// Pré-condição: canPlaceShip() retornou 1 para os mesmos parâmetros.
static void placeShip(
    int board[BOARD_SIZE][BOARD_SIZE],
    const int ship[SHIP_SIZE],
    int startRow,
    int startCol,
    ShipOrientation orientation
) {
    switch (orientation) {
        case ORIENT_HORIZONTAL:
            for (int i = 0; i < SHIP_SIZE; i++) board[startRow][startCol + i] = ship[i];
            break;
        case ORIENT_VERTICAL:
            for (int i = 0; i < SHIP_SIZE; i++) board[startRow + i][startCol] = ship[i];
            break;
        case ORIENT_DIAG_DOWN_RIGHT:
            for (int i = 0; i < SHIP_SIZE; i++) board[startRow + i][startCol + i] = ship[i];
            break;
        case ORIENT_DIAG_UP_RIGHT:
            for (int i = 0; i < SHIP_SIZE; i++) board[startRow - i][startCol + i] = ship[i];
            break;
        default:
            break;
    }
}

// Exibe o tabuleiro com espaçamento para facilitar a visualização.
static void printBoard(const int board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\nTabuleiro (0 = água, 3 = navio, 5 = habilidade):\n\n");

    // Cabeçalho de colunas
    printf("    ");
    for (int col = 0; col < BOARD_SIZE; col++) {
        printf("%2d ", col);
    }
    printf("\n");

    // Linha separadora
    printf("    ");
    for (int col = 0; col < BOARD_SIZE; col++) {
        printf("---");
    }
    printf("\n");

    for (int row = 0; row < BOARD_SIZE; row++) {
        // Índice da linha
        printf("%2d | ", row);
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("%2d ", board[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

// Constrói máscara de habilidade em forma de cone apontando para baixo (↧)
// Ponta do cone no centro da máscara; expande para linhas abaixo do centro.
static void buildConeDownMask(int mask[ABILITY_SIZE][ABILITY_SIZE]) {
    int center = ABILITY_SIZE / 2;
    for (int i = 0; i < ABILITY_SIZE; i++) {
        for (int j = 0; j < ABILITY_SIZE; j++) {
            mask[i][j] = 0;
        }
    }
    for (int d = 0; d <= center; d++) {
        int r = center + d;
        if (r >= 0 && r < ABILITY_SIZE) {
            int cStart = center - d;
            int cEnd   = center + d;
            if (cStart < 0) cStart = 0;
            if (cEnd >= ABILITY_SIZE) cEnd = ABILITY_SIZE - 1;
            for (int c = cStart; c <= cEnd; c++) {
                mask[r][c] = 1;
            }
        }
    }
}

// Constrói máscara de habilidade em forma de cruz (+), centrada
static void buildCrossMask(int mask[ABILITY_SIZE][ABILITY_SIZE]) {
    int center = ABILITY_SIZE / 2;
    for (int i = 0; i < ABILITY_SIZE; i++) {
        for (int j = 0; j < ABILITY_SIZE; j++) {
            int isCenterRow = (i == center);
            int isCenterCol = (j == center);
            mask[i][j] = (isCenterRow || isCenterCol) ? 1 : 0;
        }
    }
}

// Constrói máscara de habilidade em forma de octaedro (losango em 2D), centrada
static void buildOctahedronMask(int mask[ABILITY_SIZE][ABILITY_SIZE]) {
    int center = ABILITY_SIZE / 2;
    int radius = center;
    for (int i = 0; i < ABILITY_SIZE; i++) {
        for (int j = 0; j < ABILITY_SIZE; j++) {
            int manhattan = abs(i - center) + abs(j - center);
            mask[i][j] = (manhattan <= radius) ? 1 : 0;
        }
    }
}

// Sobrepõe uma máscara de habilidade no tabuleiro, centrando a máscara em (originRow, originCol)
// Regras: onde mask==1 e a posição está dentro do tabuleiro, marca ABILITY_MARK
// Não sobrescreve partes de navio (mantém SHIP_MARK visível)
static void overlayAbility(
    int board[BOARD_SIZE][BOARD_SIZE],
    int mask[ABILITY_SIZE][ABILITY_SIZE],
    int originRow,
    int originCol
) {
    int center = ABILITY_SIZE / 2;
    for (int i = 0; i < ABILITY_SIZE; i++) {
        for (int j = 0; j < ABILITY_SIZE; j++) {
            if (mask[i][j] == 0) continue;
            int br = originRow + (i - center);
            int bc = originCol + (j - center);
            if (br < 0 || br >= BOARD_SIZE || bc < 0 || bc >= BOARD_SIZE) continue;
            if (board[br][bc] != SHIP_MARK) {
                board[br][bc] = ABILITY_MARK;
            }
        }
    }
}

int main(void) {
    // Matriz do tabuleiro 10x10
    int board[BOARD_SIZE][BOARD_SIZE];

    // Vetores que representam navios de tamanho 3
    // Cada posição contém o valor SHIP_MARK (3), que será copiado ao tabuleiro
    int shipA[SHIP_SIZE] = { SHIP_MARK, SHIP_MARK, SHIP_MARK }; // horizontal
    int shipB[SHIP_SIZE] = { SHIP_MARK, SHIP_MARK, SHIP_MARK }; // vertical
    int shipC[SHIP_SIZE] = { SHIP_MARK, SHIP_MARK, SHIP_MARK }; // diagonal ↘
    int shipD[SHIP_SIZE] = { SHIP_MARK, SHIP_MARK, SHIP_MARK }; // diagonal ↗

    initializeBoard(board);

    // Coordenadas iniciais definidas no código (conforme simplificação)
    // Ajuste conforme desejar, desde que respeite os limites e não haja sobreposição.
    int hStartRow = 2;  // linha inicial do navio horizontal
    int hStartCol = 1;  // coluna inicial do navio horizontal
    int vStartRow = 5;  // linha inicial do navio vertical
    int vStartCol = 5;  // coluna inicial do navio vertical

    // Diagonal ↘ (down-right) começando em (0,7): (0,7),(1,8),(2,9)
    int d1StartRow = 0;
    int d1StartCol = 7;

    // Diagonal ↗ (up-right) começando em (9,0): (9,0),(8,1),(7,2)
    int d2StartRow = 9;
    int d2StartCol = 0;

    // Validação do posicionamento (dentro dos limites e sem sobreposição)
    if (!canPlaceShip(board, hStartRow, hStartCol, ORIENT_HORIZONTAL)) {
        fprintf(stderr, "Erro: navio horizontal inválido nas coordenadas (%d,%d).\n", hStartRow, hStartCol);
        return EXIT_FAILURE;
    }

    // Posiciona o navio horizontal
    placeShip(board, shipA, hStartRow, hStartCol, ORIENT_HORIZONTAL);

    // Agora valida o navio vertical considerando já o navio horizontal posicionado
    if (!canPlaceShip(board, vStartRow, vStartCol, ORIENT_VERTICAL)) {
        fprintf(stderr, "Erro: navio vertical inválido ou sobreposto nas coordenadas (%d,%d).\n", vStartRow, vStartCol);
        return EXIT_FAILURE;
    }

    // Posiciona o navio vertical
    placeShip(board, shipB, vStartRow, vStartCol, ORIENT_VERTICAL);

    // Valida e posiciona diagonal ↘
    if (!canPlaceShip(board, d1StartRow, d1StartCol, ORIENT_DIAG_DOWN_RIGHT)) {
        fprintf(stderr, "Erro: navio diagonal (↘) inválido/overlap nas coordenadas (%d,%d).\n", d1StartRow, d1StartCol);
        return EXIT_FAILURE;
    }
    placeShip(board, shipC, d1StartRow, d1StartCol, ORIENT_DIAG_DOWN_RIGHT);

    // Valida e posiciona diagonal ↗
    if (!canPlaceShip(board, d2StartRow, d2StartCol, ORIENT_DIAG_UP_RIGHT)) {
        fprintf(stderr, "Erro: navio diagonal (↗) inválido/overlap nas coordenadas (%d,%d).\n", d2StartRow, d2StartCol);
        return EXIT_FAILURE;
    }
    placeShip(board, shipD, d2StartRow, d2StartCol, ORIENT_DIAG_UP_RIGHT);

    // Exibe o tabuleiro com navios posicionados
    printBoard(board);

    // Construir máscaras de habilidades (5x5)
    int coneMask[ABILITY_SIZE][ABILITY_SIZE];
    int crossMask[ABILITY_SIZE][ABILITY_SIZE];
    int octaMask[ABILITY_SIZE][ABILITY_SIZE];

    buildConeDownMask(coneMask);
    buildCrossMask(crossMask);
    buildOctahedronMask(octaMask);

    // Definir origens das habilidades no tabuleiro (centradas)
    int coneOriginRow = 3, coneOriginCol = 3;   // cone para baixo a partir do centro
    int crossOriginRow = 6, crossOriginCol = 2; // cruz
    int octaOriginRow = 4, octaOriginCol = 7;   // losango

    // Sobrepor habilidades ao tabuleiro com validação de limites
    overlayAbility(board, coneMask, coneOriginRow, coneOriginCol);
    overlayAbility(board, crossMask, crossOriginRow, crossOriginCol);
    overlayAbility(board, octaMask, octaOriginRow, octaOriginCol);

    // Exibe o tabuleiro com habilidades
    printBoard(board);

    return EXIT_SUCCESS;
}



typedef struct Vec2{
    float x;
    float y;
} Vec2;

typedef struct Player{
    Vec2 position; //posição x e y (em pixel, x=0 e y=0 é o canto superior esquerdo)
    Vec2 velocity; //componente x é velocidade horizontal, y é vertical (em pixels/frame)
    Vec2 acceleration; //componente x é aceleração horizontal, y é vertical (em pixels/frame²)
    Vec2 size; //usado para calculos de colisão (e desenho)
} Player;
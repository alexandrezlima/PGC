# Renderização Baseada em Física em Jogos Digitais

A aplicação interativa em WebAssembly deste projeto de graduação em computação pode ser acessada em [https://alexandrezlima.github.io/PGC](https://alexandrezlima.github.io/PGC).

![Aplicação interativa WebAssembly](https://github.com/alexandrezlima/PGC/blob/main/figuras/pgc.gif)


## Contexto
A aplicação apresenta a utilização de materiais PBR sobre uma malha tridimensional. A ideia é comparar o modelo de iluminação PBR desenvolvido por este projeto com o modelo nativo presente no Unreal Engine. O modelo desenvolvido está presente na parte textual deste projeto intitulado "Renderização Baseada em Física em Jogos Digitais".

## Como usar
A comparação entre os modelos de iluminação pode ser realizada alterando o tipo de material presente no lado direito da aplicação por meio da caixa de seleção `Materiais`.

![Tipos de materiais na aplicação interativa](https://github.com/alexandrezlima/PGC/blob/main/figuras/Materiais.png)

As opções disponíveis são:
* **Custom PBR**: modelo de iluminação PBR desenvolvido no projeto.
* **Unreal PBR**: modelo de iluminação PBR nativo do Unreal Engine.
* **Phong**: modelo de iluminação de Phong com sombreamento Phong baseado no modelo apresentado [neste link](https://hbatagelo.github.io/cg/phongmodel.html).

Alterar o valor desta caixa de seleção, portanto, alterará o modelo de iluminação utilizado pela malha 3D.

### Cores e Texturas
A malha tridimensional, a princípio, apresenta uma cor sólida que pode ser controlada pelos _sliders_ RGB do lado esquerdo da aplicação.

![Cores da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/Cores.png)

A _checkbox_ `Use textures` habilita a seleção de texturas que serão aplicadas sobre a malha no lugar da cor sólida. Algumas dessas texturas possuem mapas de normais e mapas de metalicidade ou rugosidade, aumentando o realismo do material analisado.

![Texturas da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/Texturas.png)

Para alterar o tipo de textura, basta selecionar a desejada clicando sobre uma das figuras apresentadas acima.

### Rugosidade
O parâmetro de `rugosidade` está disponível apenas para os modelos de iluminação PBR. Alterar o valor deste slider fará com que a percepção de rugosidade da superfície seja maior ou menor. Este slider não terá efeito caso a malha 3D esteja utilizando alguma textura com mapa de rugosidade.

![Texturas da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/Roughness.png)

### Metalicidade
O parâmetro de `metalicidade` está disponível apenas para os modelos de iluminação PBR. Alterar o valor deste slider fará com que a percepção de metalicidade da superfície seja maior ou menor. Este slider não terá efeito caso a malha 3D esteja utilizando alguma textura com mapa de metalicidade.

![Texturas da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/Metallic.png)

### Funções F, D e G
O modelo **Custom PBR** permite que as funções que compõem a BRDF do modelo de iluminação sejam alteradas em tempo de execução. Esses valores podem ser alterados nas caixas de seleção presentes no lado direito da aplicação.

![Texturas da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/FDG.png)

* F, função Fresnel: utiliza a aproximação Fresnel-Schlick ou uma versão sem Fresnel.
* D, função de distribuição: GGX (Trowbridge-Reitz), Blinn-Phong, Beckmann.
* G, função geométrica: GGX, Implicit, Neumann, Cook-Torrance, Keleman, Schlick-GGX.

### Malha 3D
A malha tridimensional visualizada pode ser alterada em tempo de execução clicando em um dos objetos disponíveis nos botões do painel inferior direito.

![Texturas da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/Malha.png)

O objeto pode ser rotacionado caso o botão esquerdo do _mouse_ seja pressionado e segurado e o _mouse_ movido para direção desejada.

### Iluminação
A aplicação conta com uma fonte de iluminação direta. Sua direção pode ser alterada caso o botão direito do _mouse_ seja pressionado e segurado e o _mouse_ movido para direção desejada. A direção da iluminação pode ser observada no canto inferior esquerdo.

![Texturas da malha 3D](https://github.com/alexandrezlima/PGC/blob/main/figuras/Iluminacao.png)

### Zoom
O objeto pode ser aproximado ou afastado utilizando o _scroll_ do mouse.



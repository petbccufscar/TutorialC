# Testes
Lista de testes de interação. Inclui comportamentos que anteriormente eram incorretos e foram consertados.
Favor verificar se após adições novas esses bugs não voltam a acontecer...

## Blocos
- Um bloco criado com o campo `permanent = true` não deve ser deletado  (e.g blocos criados com o botão `Criar` no painel de testes)
- Ao passar o mouse por cima de um bloco, ele deve apresentar mudança na cor
- Outra mudança deve ocorrer ao pressionar (arrastar) o bloco
- Arrastar o bloco move o bloco, o cursor deve permanecer na mesma posição em relação ao bloco
- Na situação de um bloco estar acima do outro visualmente, a seleção deve seguir a ordem (i.e o bloco mais a frente deve ser selecionado se o mouse estiver em cima dele, e não o bloco atrás deste)

## Geradores
- Apertar, segurar e arrastar o mouse a partir de um gerador de blocos exibe um novo bloco abaixo do mouse
    - Ao ser solto o mouse em uma região vazia, o bloco anteriormente segurado deve sumir
    - O mesmo deve acontecer se o bloco for solto em cima de outro gerador

## Campos
- Ao arrastar um bloco acima do campo, o bloco deve permanecer existindo, e o campo deve aumentar de acordo com o tamanho do bloco
- Ao retirar um bloco do campo, em direção a uma região vazia, o bloco deve sumir e o campo deve diminuir
- Ao retirar um bloco do campo, em direção a outro campo, o campo original deve diminuir, o outro campo deve aumentar para acomodar o bloco e bloco deve continuar existindo
- Caso um bloco seja arrastado em direção a um campo que aparenta já ter um bloco, o novo bloco não deve apresentar o comportamento de encaixe no campo.
    - Futuramente blocos arrastados de um campo para outro devem trocar de posição
- Arrastar um bloco de dentro do campo para dentro desse mesmo campo (porém em posição diferente dentro do campo) deve encaixar novamente no local original (i.e Se o bloco é arrastado, mas o mouse ainda está dentro do campo, ao soltar o mouse o bloco volta para a posição correta dentro do campo)
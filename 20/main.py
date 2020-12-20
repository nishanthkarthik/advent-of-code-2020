import numpy as np

monster = np.asarray([[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, ],
                      [1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, ],
                      [0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, ]], dtype=np.int)
monster_count = np.sum(monster)


def make_monsters():
    arr = []
    m = monster
    arr.append(m)

    for i in range(3):
        m = np.rot90(m)
        arr.append(m)

    m = np.fliplr(m)
    arr.append(m)
    for i in range(3):
        m = np.rot90(m)
        arr.append(m)
    return arr


def read_img():
    arr = np.genfromtxt("/ssd/build-advent-of-code-2020/20/image.txt", delimiter=",", dtype=np.int)
    return arr[:, :-1]


img = read_img()
print(img.shape)
monsters = make_monsters()

total_monsters = 0

for monster in monsters:
    i, j = monster.shape
    I, J = img.shape
    for ii in range(0, I - i + 1):
        for jj in range(0, J - j + 1):
            tile = img[ii:(ii + i), jj:(jj + j)]
            res = np.sum(np.multiply(monster, tile))
            if res >= monster_count:
                total_monsters += 1

print(total_monsters, np.sum(img) - monster_count * total_monsters)

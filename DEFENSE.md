# Guide de défense — Codexion

> Document personnel de préparation à la soutenance (en français).
> Le README officiel du projet est `README.md` (en anglais, exigé par le sujet).

---

## 0. Le projet en une phrase

Des **coders** (= des threads) tournent en boucle : **compiler → débuguer → refactorer**.
Pour compiler, un coder a besoin de **2 dongles** en même temps (celui de gauche et
celui de droite). Il y a autant de dongles que de coders, en cercle, donc les voisins
se **partagent** les dongles. Si un coder n'arrive pas à recompiler à temps, il
**burnout**. Un **thread monitor** surveille tout le monde et arrête la simulation.

C'est une variante du problème classique du **dîner des philosophes** (philosophes →
coders, fourchettes → dongles, manger → compiler, mourir → burnout).

---

## 1. Les acteurs (les threads)

| Thread | Combien | Rôle |
|---|---|---|
| **main** | 1 | parse les arguments, construit tout, crée les threads, les attend (`join`), libère la mémoire |
| **coder** (`run_coder`) | N (un par coder) | la boucle compile/debug/refactor |
| **monitor** (`run_monitor`) | 1 | détecte le burnout, détecte la fin, pose le flag d'arrêt |

**Idée clé** : les coders ne se parlent **jamais** entre eux. Ils communiquent seulement
en **partageant de la mémoire** (l'état des dongles, leur progression). Le monitor est le
seul à avoir la vue d'ensemble et le droit d'arrêter.

---

## 2. Les structures de données (qui contient quoi)

Tout est rangé dans **une seule structure `t_env`** passée par **pointeur** à tous les
threads → **aucune variable globale** (interdites par le sujet).

- **`t_request`** : une demande de dongle. Contient `ticket` (ordre d'arrivée) et
  `deadline` (= `last_compile_start + time_to_burnout`, pour l'EDF).
- **`t_dongle`** : un dongle. Contient son `lock` (mutex), sa `cond` (condition), son
  `heap` (la file d'attente), `is_taken` (occupé ?), `released_at` (dernière libération),
  `next_ticket`, `heap_size`.
- **`t_coder`** : un coder. Contient `id`, `dongle_left`/`dongle_right`,
  `last_compile_start`, `compiles_done`, son `data_lock`, et un pointeur vers `env`.
- **`t_env`** : l'environnement global. Les paramètres (temps, scheduler…), le flag
  `stop`, `start_time`, `stop_lock`, `log_lock`, le tableau des dongles, le tableau des
  coders, le `monitor_thread`.

---

## 3. Le cycle de vie d'un coder (`run_coder`)

```
tant que (pas d'arrêt) :
    choose_dongle      -> choisir mes 2 dongles (et dans quel ordre)
    take_dongle(first)  -> prendre le 1er  (sinon stop -> on sort)
    take_dongle(second) -> prendre le 2e   (sinon stop -> on libère le 1er et on sort)
    compile            -> compiler, puis RELÂCHER les 2 dongles
    debug_and_refactor -> débuguer puis refactorer
```

Détails utiles :
- `choose_dongle` choisit toujours **le dongle de plus petit numéro en premier** (= la
  clé anti-deadlock, voir §8).
- C'est `take_dongle` qui affiche `has taken a dongle` (une fois par dongle pris).
- `compile` met à jour `last_compile_start`, affiche `is compiling`, attend
  `time_to_compile`, incrémente `compiles_done`, puis **relâche les 2 dongles**.

---

## 4. Comment on prend un dongle (`take_dongle` + `grant_dongle`)

`take_dongle` = « je réclame UN dongle et j'attends de le tenir vraiment ».

```
verrouiller le dongle
prendre un ticket (mon numéro d'arrivée)
calculer ma deadline
me mettre dans la file (heap_push)
tant que (pas d'arrêt) :
    si (dongle libre  ET  je suis au sommet du heap) :
        grant_dongle()              -> je le prends pour de vrai
        log "has taken a dongle"
        return 1
    sinon : cond_wait               -> je dors, on me réveillera
return 0   (arrêt demandé)
```

`grant_dongle` = « le moment de la prise » :
```
heap_pop      -> je sors de la file (je ne suis plus en attente)
is_taken = 1  -> je RÉSERVE le dongle (personne d'autre ne peut le prendre)
released = released_at  (photo, sous le lock)
unlock        -> je relâche le mutex (ma réservation suffit à bloquer les autres)
cooldown_sleep(released, cooldown)  -> j'attends le cooldown restant, HORS du lock
```

**Pourquoi `is_taken` ?** Parce que le détenteur **sort du heap** (`heap_pop`) quand il
prend le dongle. Du coup, le heap ne contient que les **gens en attente**, pas le
détenteur. `is_taken` est donc l'info « occupé » qui manque. Sans lui, le voisin
verrait un heap (où il est seul) et croirait le dongle libre → double prise.

---

## 5. Le heap binaire — et pourquoi « le parent a toujours raison »

### C'est quoi
Un **tableau** lu comme un **arbre** : pour la case `i`, le parent est `(i-1)/2`, les
enfants sont `2i+1` et `2i+2`. Chaque dongle a son propre heap (sa file d'attente).

### La règle d'or (l'invariant)
> **Tout parent est plus prioritaire que ses enfants.**

Conséquence : par transitivité, le **sommet (`heap[0]`) est le plus prioritaire de tous**.
C'est ça « le parent a toujours raison » : si je fais confiance au fait que chaque parent
bat ses 2 enfants, alors **le sommet bat tout le monde**, sans avoir à comparer tout le
tableau. Le gagnant est **toujours** en `heap[0]`.

### Comment on garde la règle vraie
- **`heap_push` (ajout)** → on pose le nouveau **en bas**, puis **`bubble_up`** : il
  remonte tant qu'il est plus prioritaire que son parent.
- **`heap_pop` (retrait du sommet)** → on met le **dernier** élément au sommet (pour
  reboucher le trou), puis **`bubble_down`** : il redescend en s'échangeant avec **le
  plus prioritaire de ses 2 enfants**.

`bubble_up` et `bubble_down` font des **échanges locaux** (parent ↔ enfant). Mais comme
chaque échange respecte la règle localement, l'invariant global reste vrai. C'est ça
l'astuce : on ne trie jamais tout, on corrige juste le long d'un chemin (en O(log n)).

### Chez moi : 2 éléments max
Un dongle n'a que **2 voisins** comme prétendants, et chacun a **1 demande en attente**
à la fois → le heap fait **au plus 2 cases** (`HEAP_CAP = 2`). Donc `bubble_up` fait au
plus 1 comparaison et `bubble_down` ne descend jamais. **Mais** le code reste un **vrai
heap générique** (exigé par le sujet : « implement a priority queue (heap) »).

---

## 6. FIFO vs EDF — une seule ligne qui change tout

C'est la fonction de comparaison `request_is_priority(a, b, scheduler)` qui définit « plus
prioritaire » :

```c
if (scheduler == SCHED_EDF && a->deadline != b->deadline)
    return (a->deadline < b->deadline);   // EDF : deadline la plus proche gagne
return (a->ticket < b->ticket);           // FIFO (ou égalité EDF) : 1er arrivé gagne
```

- **FIFO** = ordre d'arrivée (le plus petit `ticket`).
- **EDF** = le plus urgent (la plus petite `deadline` = celui qui va burnout le plus tôt) ;
  en cas d'égalité, on retombe sur le `ticket` (pour rester **déterministe**, exigé par le
  sujet).

Le heap est **agnostique** : il met toujours « le plus prioritaire » en haut. Seul le
**sens** de « prioritaire » change avec le scheduler.

---

## 7. Pourquoi on lock, et quoi exactement

**Règle générale** : il faut un mutex dès qu'une donnée est **partagée entre threads** ET
**modifiée** par au moins un. Sinon = **data race** = comportement indéfini (le compilateur
peut mettre la valeur en cache, ne jamais voir le changement, lire une valeur à moitié
écrite…). Une donnée **lue seulement** (jamais modifiée après l'init) **n'a pas besoin de
lock**.

### Les 4 verrous et ce qu'ils protègent

| Mutex | Protège | Partagé entre |
|---|---|---|
| **`dongle->lock`** (1 par dongle) + **`dongle->cond`** | l'état du dongle : `is_taken`, `released_at`, son `heap`, `heap_size`, `next_ticket` | les 2 coders voisins |
| **`coder->data_lock`** (1 par coder) | `last_compile_start`, `compiles_done` | le coder (écrit) ↔ le monitor (lit) |
| **`stop_lock`** | le flag `stop` | le monitor (écrit) ↔ tous les coders (lisent) |
| **`log_lock`** | l'affichage (`printf`) | tous les threads |

### Données SANS lock (lecture seule après init)
Les temps (`time_to_compile`…), `scheduler`, `nb_coders`, `start_time`, `dongle_left/right`,
`id` : posés **une fois avant** le lancement des threads, puis **seulement lus** → pas de
data race → pas besoin de lock. (Bon argument à donner : *« je ne lock que ce qui est
partagé ET modifié »*.)

### Exemple de race évitée
Deux voisins partagent un dongle. Sans lock, les deux pourraient lire `is_taken == 0` au
même instant et le prendre tous les deux. Chez moi : le test `is_taken == 0` **et** le
`is_taken = 1` se font **sous `dongle->lock`**, et seul le coder au **sommet du heap** a le
droit de le faire. Le voisin ne peut même pas lire l'état du dongle tant que le premier n'a
pas relâché le mutex → il voit `is_taken == 1` → il attend.

### Pourquoi `should_stop()` au lieu de lire `env->stop` directement
`stop` est partagé (le monitor l'écrit, les coders le lisent). Le lire sans lock = race :
le compilateur pourrait le mettre en cache dans un registre et **boucler à l'infini**.
`should_stop()` fait lock/lecture/unlock → lecture sûre et à jour.

---

## 8. La prévention du deadlock (les 4 conditions de Coffman)

Un deadlock a besoin de **4 conditions réunies**. J'en casse **une** :

1. **Exclusion mutuelle** : un dongle = un seul détenteur. *(inévitable ici)*
2. **Hold and wait** : un coder prend 1 dongle puis attend le 2e. *(présent chez moi)*
3. **Pas de préemption** : on ne vole pas un dongle tenu. *(présent)*
4. **Attente circulaire** : ❌ **CASSÉE.** Tous les coders prennent **le dongle de plus
   petit numéro en premier** (`choose_dongle`). Avec cet **ordre global**, aucun cycle
   « A attend B qui attend A » ne peut se former.

**Phrase à dire** : *« J'évite le deadlock par ordonnancement des ressources : chaque
coder demande toujours le dongle de plus petit indice d'abord. Comme tout le monde suit le
même ordre, il ne peut pas y avoir d'attente circulaire. »*

C'est la solution classique du dîner des philosophes (le philosophe pair/impair, ou ici
l'ordre par numéro).

---

## 9. Le cooldown

> *« Après libération, un dongle est indisponible pendant `dongle_cooldown` ms. »*

Quand un coder **gagne** un dongle, avant de l'utiliser, il regarde depuis combien de temps
il a été **relâché** (`released_at`). Si moins de `cooldown` ms se sont écoulées, il dort le
**temps restant** (`cooldown_sleep`) — **hors du lock**, car il a déjà réservé (`is_taken=1`),
donc personne d'autre ne peut le prendre pendant qu'il dort.

Cas de la **1ʳᵉ prise** : `released_at = 0` (init), et `get_time_in_ms()` est un temps
**absolu** énorme → `cooldown - (now - 0)` est négatif → **pas d'attente**. Élégant : pas
besoin de cas spécial, le calcul gère « jamais relâché » tout seul.

---

## 10. Le burnout et le monitor

- Un coder **burnout** s'il ne **redémarre pas une compilation** dans les
  `time_to_burnout` ms depuis le début de sa **dernière** compile (ou depuis le début).
- Le **monitor** boucle et vérifie chaque coder **toutes les ~0,5 ms** :
  `elapsed = now - last_compile_start ; si elapsed > time_to_burnout → burnout`.
- 0,5 ms de poll → détection **bien en-dessous** des **10 ms** exigés par le sujet.
- Au burnout : `log_burnout` (affiche `burned out`) **puis** `set_stop`.
- Le monitor vérifie aussi `check_all_c_done` : si **tous** ont atteint le quota → `set_stop`
  (fin par succès, sans burnout).

---

## 11. L'arrêt propre (2 pièges réglés)

**Piège 1 — le réveil perdu (lost wakeup).** Un coder peut dormir dans `cond_wait`. Si on
posait juste `stop = 1` sans le réveiller, il dormirait pour toujours → `join` qui bloque.
Solution : `set_stop` fait `pthread_cond_broadcast` sur **chaque dongle**, **sous le lock du
dongle**. Comme le coder tient ce lock pendant qu'il teste `should_stop` puis `cond_wait`,
le broadcast ne peut **pas** passer « entre les deux » → personne ne rate le réveil.

**Piège 2 — un log après `burned out`.** `log_event` **n'affiche rien si `stop` est posé**.
Donc une fois le burnout affiché et `stop` posé, plus aucune ligne d'état ne sort. Le
`burned out` est **toujours la dernière ligne**.

Résultat : `main` fait `pthread_join` sur tous les coders puis le monitor, et le programme
se termine proprement.

---

## 12. Le cas particulier : 1 seul coder

1 coder = 1 dongle. Or `dongle_left = 0` et `dongle_right = (0+1) % 1 = 0` → **le même
dongle**. Le coder le prend une fois (`has taken a dongle`), puis essaie d'en prendre un 2e
(le même, déjà `is_taken=1`) → il **attend pour toujours** → il **burnout** après
`time_to_burnout`. Sortie :
```
0   1 has taken a dongle
401 1 burned out
```
Conforme au sujet : 1 dongle ne suffit pas pour compiler (il en faut 2). **Pas de cas
spécial dans le code** : la formule `(i+1) % nb` le produit tout seul.

---

## 13. La gestion mémoire (zéro fuite)

Alloué sur le **tas** : le tableau `dongles`, le tableau `coders`, et **un `heap` par
dongle**. `run_destroy` libère **chaque `heap`**, puis les 2 tableaux, et détruit tous les
mutex/cond. Le `setting` est sur la **pile** (pas de `malloc`, donc pas de `free`).
Vérifié à l'**AddressSanitizer** : 0 fuite, 0 crash.

---

## 14. Questions probables du correcteur (+ réponses)

**« Pourquoi un mutex PAR dongle et pas un seul gros mutex ? »**
Pour le parallélisme : des coders sur des dongles **différents** peuvent avancer en même
temps. Un seul mutex global sérialiserait tout.

**« Pourquoi une variable de condition et pas un `usleep` dans une boucle ? »**
`cond_wait` endort le thread **sans consommer de CPU** et le réveille **pile** quand un
dongle se libère (broadcast). Un `usleep` en boucle ferait du *busy-waiting* (CPU gaspillé)
et réagirait moins vite.

**« Comment tu évites le deadlock ? »**
Ordre des ressources : toujours le dongle de plus petit numéro d'abord → pas d'attente
circulaire (condition de Coffman cassée).

**« Pourquoi le détenteur sort du heap et tu utilises `is_taken` ? »**
Si le détenteur restait dans le heap, en EDF une demande plus urgente arrivée **après**
pourrait passer **devant** lui dans le heap → 2 coders croiraient avoir le dongle. En sortant
le détenteur et en marquant `is_taken`, l'occupation est claire et l'exclusion garantie.

**« Pourquoi `last_compile_start` et pas `released_at` pour le burnout ? »**
Le burnout mesure le temps depuis le **début de la dernière compilation**, pas depuis la
libération d'un dongle. Ce sont deux notions différentes : `last_compile_start` = horloge du
coder ; `released_at` = horloge du dongle (pour le cooldown).

**« Pourquoi `printf` est protégé ? »**
Sinon deux threads pourraient écrire **au milieu** de la ligne de l'autre. `log_lock`
sérialise → une ligne complète à la fois.

**« Burnout détecté en combien de temps ? »**
Poll toutes les ~0,5 ms → bien sous les 10 ms demandés.

**« Et si je mets `dongle_cooldown` énorme ? »**
Les coders attendent plus longtemps entre deux prises → ils risquent de burnout si
`time_to_burnout` est trop petit. C'est cohérent avec le modèle.

**« Montre-moi où tu protèges une race précise. »**
Le test `is_taken == 0` puis `is_taken = 1` dans `take_dongle`/`grant_dongle`, tous deux sous
`dongle->lock`. (Voir §7.)

**« Pourquoi pas de variable globale ? »**
Interdit par le sujet, et tout est dans `t_env` passé par pointeur.

---

## 15. Commandes de test utiles

```bash
# Succès (tous compilent 3 fois, personne ne burnout)
./codexion 5 800 200 200 200 3 0 fifo

# Burnout (fenêtre trop courte)
./codexion 5 250 100 100 100 999 0 edf

# 1 coder (prend 1 dongle puis burnout)
./codexion 1 400 200 200 200 5 0 fifo

# Cooldown visible (reprise = release + cooldown)
./codexion 2 2000 50 50 50 5 200 fifo

# Arguments invalides (tous rejetés)
./codexion -5 200 20 20 20 2 0 fifo
./codexion 5 200 20 20 20 2 0 lifo
```

Outils de vérif :
- `norminette` → 0 erreur.
- `cc -fsanitize=address` → 0 fuite, 0 crash.
- `cc -fsanitize=thread` → 0 data race.

---

## 16. Le résumé à retenir (les 5 phrases clés)

1. **Threads** : 1 par coder + 1 monitor ; ils communiquent par **mémoire partagée**, pas
   en s'appelant.
2. **Deadlock** : cassé par **l'ordre des dongles** (plus petit numéro d'abord).
3. **Arbitrage** : un **heap par dongle**, le sommet est toujours le gagnant (FIFO = 1er
   arrivé, EDF = plus urgent).
4. **Locks** : on protège **uniquement** ce qui est **partagé et modifié** (dongle, données
   du coder, flag stop, affichage) ; le reste est en lecture seule.
5. **Arrêt** : le monitor pose `stop` + réveille tout le monde (broadcast) ; plus aucun log
   après `burned out` ; tout est `join` et libéré.

# studium_db — Gestion des étudiants

Application de bureau de gestion d'étudiants développée en **C** avec **GTK+ 3** (interface graphique) et **MySQL** (base de données). Mini-projet réalisé dans le cadre du module **Algorithmique et Programmation C** (1er semestre), encadré par le Pr. Hakim El Massari.

## Fonctionnalités

| Action | Description |
|---|---|
| **Ajouter** | Enregistre un nouvel étudiant (nom et CNE obligatoires) |
| **Modifier** | Met à jour les informations d'un étudiant identifié par son CNE |
| **Supprimer** | Supprime un étudiant à partir de son CNE |
| **Rechercher** | Recherche par CNE (exact) ou par nom (partiel) |
| **Afficher tout** | Liste tous les étudiants dans la fenêtre |

Champs gérés : nom, prénom, CNE, date de naissance, ville, adresse, compte académique, filière, année d'étude.

## Prérequis

- Compilateur **GCC**
- **GTK+ 3** (paquets de développement)
- **MySQL** ou **MariaDB** (serveur + bibliothèque cliente de développement)
- `make` et `pkg-config`

Installation sous Debian/Ubuntu :

```bash
sudo apt install build-essential pkg-config libgtk-3-dev libmysqlclient-dev mysql-server
```

Sous Windows, utilisez MSYS2 (`pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-libmariadbclient`) ou l'environnement Code::Blocks avec GTK+ configuré.

## Installation

```bash
git clone https://github.com/<votre-compte>/studium-db.git
cd studium-db

# 1. Créer la base et la table
mysql -u root -p < schema.sql

# 2. Compiler
make
```

## Configuration

La connexion à la base se règle par variables d'environnement (valeurs par défaut entre parenthèses) :

| Variable | Rôle |
|---|---|
| `DB_HOST` | Hôte MySQL (`localhost`) |
| `DB_PORT` | Port (`3306`) |
| `DB_USER` | Utilisateur (`root`) |
| `DB_PASSWORD` | Mot de passe (vide) |
| `DB_NAME` | Nom de la base (`studium_db`) |

Exemple : `DB_USER=etu DB_PASSWORD=secret ./studium_db_app`
Un modèle est fourni dans `.env.example`. Ne commitez jamais vos vrais identifiants.

## Utilisation

```bash
./studium_db_app
```

1. Remplissez les champs puis cliquez sur **Ajouter**.
2. Pour **Modifier**, **Supprimer** ou **Rechercher**, renseignez le CNE (ou le nom pour la recherche).
3. **Afficher tout** liste les étudiants dans la zone du bas.

## Structure du projet

```
studium-db/
├── src/main.c        # Code source de l'application
├── schema.sql        # Création de la base et de la table students
├── Makefile          # Compilation
├── .env.example      # Modèle de configuration
└── README.md
```

## Outils utilisés

Code::Blocks (IDE), MySQL (SGBD), GTK+ (interface), GCC (compilateur), GDB (débogueur).

## Auteurs

- Chaymae El Bahloul
- Khadija El Basri
- Samah Boudallaa
- Khadija Rizqy
- Yassamine Obba

Encadrant : Pr. Hakim El Massari

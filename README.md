# studium_db — Gestion des étudiants

[![Langage](https://img.shields.io/badge/Langage-C-A8B9CC)]()
[![GTK](https://img.shields.io/badge/Interface-GTK%2B%203-4A86CF)]()
[![MySQL](https://img.shields.io/badge/Base%20de%20donn%C3%A9es-MySQL%20%2F%20MariaDB-4479A1)]()
[![Projet](https://img.shields.io/badge/Projet-Mini--projet%20acad%C3%A9mique-blue)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Application de bureau de gestion d'étudiants développée en **C** avec **GTK+ 3** (interface graphique) et **MySQL** (base de données). Mini-projet réalisé dans le cadre du module **Algorithmique et Programmation C** (1er semestre), encadré par le Pr. Hakim El Massari.

## Sommaire

- [Fonctionnalités](#fonctionnalités)
- [Aperçu](#aperçu)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Configuration](#configuration)
- [Utilisation](#utilisation)
- [Structure du projet](#structure-du-projet)
- [Dépannage](#dépannage)
- [Limites et perspectives](#limites-et-perspectives)
- [Outils utilisés](#outils-utilisés)
- [Licence](#licence)
- [Auteurs](#auteurs)

## Fonctionnalités

| Action | Description |
|---|---|
| **Ajouter** | Enregistre un nouvel étudiant (nom et CNE obligatoires) |
| **Modifier** | Met à jour les informations d'un étudiant identifié par son CNE |
| **Supprimer** | Supprime un étudiant à partir de son CNE |
| **Rechercher** | Recherche par CNE (exact) ou par nom (partiel) |
| **Afficher tout** | Liste tous les étudiants dans la fenêtre |

Champs gérés : nom, prénom, CNE, date de naissance, ville, adresse, compte académique, filière, année d'étude.

## Aperçu

<!-- Ajoutez une capture d'écran de l'application dans docs/ puis décommentez la ligne suivante -->
<!-- ![Interface de studium_db](docs/screenshot.png) -->

## Prérequis

- Compilateur **GCC**
- **GTK+ 3** (paquets de développement)
- **MySQL** ou **MariaDB** (serveur + bibliothèque cliente de développement)
- `make` et `pkg-config`

Installation sous Debian/Ubuntu :

```bash
sudo apt install build-essential pkg-config libgtk-3-dev libmysqlclient-dev mysql-server
```

> Sur les versions de Debian qui n'offrent pas `libmysqlclient-dev`, installez `libmariadb-dev` à la place.

Sous Windows, utilisez MSYS2 :

```bash
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-libmariadbclient
```

ou l'environnement Code::Blocks avec GTK+ configuré.

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

Exemple :

```bash
DB_USER=etu DB_PASSWORD=secret ./studium_db_app
```

Un modèle est fourni dans `.env.example`.

> **Sécurité** : ne commitez jamais vos vrais identifiants. Vérifiez que `.env` figure dans votre `.gitignore`.

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
├── LICENSE           # Licence MIT
└── README.md
```

## Dépannage

| Problème | Piste de solution |
|---|---|
| `gtk/gtk.h` ou `mysql.h` introuvable à la compilation | Installer les paquets de développement (`libgtk-3-dev`, `libmysqlclient-dev` ou `libmariadb-dev`) |
| Échec de connexion à la base | Vérifier que le serveur MySQL est démarré et que `DB_HOST`, `DB_PORT`, `DB_USER`, `DB_PASSWORD` sont corrects |
| `Unknown database 'studium_db'` | Exécuter `mysql -u root -p < schema.sql` |
| `Access denied for user` | Vérifier l'utilisateur et le mot de passe, ainsi que ses droits sur la base |
| Aucune fenêtre ne s'affiche (Linux, WSL) | Vérifier qu'un serveur d'affichage est disponible (variable `DISPLAY`, WSLg ou X server) |

## Limites et perspectives

- Pas de système d'authentification des utilisateurs de l'application.
- Utilisation de requêtes préparées (`mysql_stmt_*`) recommandée pour toutes les opérations, afin de se prémunir contre les injections SQL.
- Pistes d'évolution : export CSV/PDF, tri et pagination de la liste, validation des formats (CNE, dates), gestion des notes et des absences.

## Outils utilisés

Code::Blocks (IDE), MySQL (SGBD), GTK+ (interface), GCC (compilateur), GDB (débogueur).

## Licence

Ce projet est distribué sous licence **MIT**. Voir le fichier [LICENSE](LICENSE) pour le texte complet.

## Auteurs

- BOUDALLAA Samah
- EL BAHLOUL Chaymae
- EL BASRI Khadija
- OBBA Yassamine
- RIZQY Khadija

**Encadrant** : Pr. Hakim El Massari

/*
 * studium_db - Gestion des étudiants (C + GTK3 + MySQL)
 * Fonctions : Ajouter, Modifier, Supprimer, Rechercher, Afficher
 */
#include <gtk/gtk.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef G_APPLICATION_DEFAULT_FLAGS   /* compatibilité GLib < 2.74 */
#define G_APPLICATION_DEFAULT_FLAGS G_APPLICATION_FLAGS_NONE
#endif

#define NB_CHAMPS 9
#define IDX_CNE 2

static const char *LABELS[NB_CHAMPS] = {
    "Nom", "Prenom", "CNE", "Date de naissance (AAAA-MM-JJ)", "Ville",
    "Adresse", "Compte Academique", "Filiere", "Annee d'etude"
};

/* Noms des colonnes dans la table `students` (même ordre que LABELS) */
static const char *COLONNES[NB_CHAMPS] = {
    "nom", "prenom", "cne", "date_naissance", "ville",
    "adresse", "compte_academique", "filiere", "annee_etudes"
};

typedef struct {
    GtkWidget *window;
    GtkWidget *entries[NB_CHAMPS];
    GtkWidget *output;          /* GtkTextView pour l'affichage */
} AppWidgets;

/* ---------- Utilitaires ---------- */

static const char *env_or(const char *name, const char *fallback) {
    const char *v = getenv(name);
    return (v && *v) ? v : fallback;
}

/* Connexion à MySQL. Les paramètres viennent des variables d'environnement. */
static MYSQL *connecter_bd(void) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) return NULL;

    const char *host = env_or("DB_HOST", "localhost");
    const char *user = env_or("DB_USER", "root");
    const char *pass = env_or("DB_PASSWORD", "");
    const char *name = env_or("DB_NAME", "studium_db");
    unsigned int port = (unsigned int)atoi(env_or("DB_PORT", "3306"));

    if (!mysql_real_connect(conn, host, user, pass, name, port, NULL, 0)) {
        fprintf(stderr, "Erreur de connexion MySQL : %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }
    mysql_set_character_set(conn, "utf8mb4");
    return conn;
}

static void message(AppWidgets *app, GtkMessageType type, const char *texte) {
    GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "%s", texte);
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

/* Retourne une copie échappée (à libérer avec g_free) : protège contre l'injection SQL. */
static char *echapper(MYSQL *conn, const char *s) {
    size_t len = strlen(s);
    char *out = g_malloc(len * 2 + 1);
    mysql_real_escape_string(conn, out, s, (unsigned long)len);
    return out;
}

static const char *champ(AppWidgets *app, int i) {
    return gtk_entry_get_text(GTK_ENTRY(app->entries[i]));
}

static void ecrire_sortie(AppWidgets *app, const char *texte) {
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->output));
    gtk_text_buffer_set_text(buf, texte, -1);
}

static void afficher_resultat(AppWidgets *app, MYSQL_RES *res) {
    GString *s = g_string_new(NULL);
    MYSQL_ROW row;
    unsigned int n = 0;
    while ((row = mysql_fetch_row(res))) {
        g_string_append_printf(s, "%-15s %-15s %-12s %-20s %s\n",
            row[0] ? row[0] : "", row[1] ? row[1] : "", row[2] ? row[2] : "",
            row[3] ? row[3] : "", row[4] ? row[4] : "");
        n++;
    }
    if (n == 0) g_string_assign(s, "Aucun resultat.");
    ecrire_sortie(app, s->str);
    g_string_free(s, TRUE);
}

/* ---------- Actions ---------- */

static void ajouter(GtkWidget *w, gpointer data) {
    AppWidgets *app = data;
    if (!*champ(app, 0) || !*champ(app, IDX_CNE)) {
        message(app, GTK_MESSAGE_WARNING, "Le nom et le CNE sont obligatoires.");
        return;
    }
    MYSQL *conn = connecter_bd();
    if (!conn) { message(app, GTK_MESSAGE_ERROR, "Connexion a la base impossible."); return; }

    GString *cols = g_string_new(NULL), *vals = g_string_new(NULL);
    for (int i = 0; i < NB_CHAMPS; i++) {
        char *e = echapper(conn, champ(app, i));
        g_string_append_printf(cols, "%s%s", i ? ", " : "", COLONNES[i]);
        g_string_append_printf(vals, "%s'%s'", i ? ", " : "", e);
        g_free(e);
    }
    char *q = g_strdup_printf("INSERT INTO students (%s) VALUES (%s)", cols->str, vals->str);

    if (mysql_query(conn, q))
        message(app, GTK_MESSAGE_ERROR, mysql_error(conn));
    else
        message(app, GTK_MESSAGE_INFO, "Etudiant ajoute avec succes.");

    g_free(q);
    g_string_free(cols, TRUE);
    g_string_free(vals, TRUE);
    mysql_close(conn);
}

static void modifier(GtkWidget *w, gpointer data) {
    AppWidgets *app = data;
    if (!*champ(app, IDX_CNE)) {
        message(app, GTK_MESSAGE_WARNING, "Indiquez le CNE de l'etudiant a modifier.");
        return;
    }
    MYSQL *conn = connecter_bd();
    if (!conn) { message(app, GTK_MESSAGE_ERROR, "Connexion a la base impossible."); return; }

    GString *set = g_string_new(NULL);
    for (int i = 0; i < NB_CHAMPS; i++) {
        if (i == IDX_CNE) continue;
        char *e = echapper(conn, champ(app, i));
        g_string_append_printf(set, "%s%s='%s'", set->len ? ", " : "", COLONNES[i], e);
        g_free(e);
    }
    char *cne = echapper(conn, champ(app, IDX_CNE));
    char *q = g_strdup_printf("UPDATE students SET %s WHERE cne='%s'", set->str, cne);

    if (mysql_query(conn, q))
        message(app, GTK_MESSAGE_ERROR, mysql_error(conn));
    else if (mysql_affected_rows(conn) == 0)
        message(app, GTK_MESSAGE_WARNING, "Aucun etudiant modifie (CNE introuvable ou donnees identiques).");
    else
        message(app, GTK_MESSAGE_INFO, "Etudiant modifie avec succes.");

    g_free(q); g_free(cne);
    g_string_free(set, TRUE);
    mysql_close(conn);
}

static void supprimer(GtkWidget *w, gpointer data) {
    AppWidgets *app = data;
    if (!*champ(app, IDX_CNE)) {
        message(app, GTK_MESSAGE_WARNING, "Indiquez le CNE de l'etudiant a supprimer.");
        return;
    }
    MYSQL *conn = connecter_bd();
    if (!conn) { message(app, GTK_MESSAGE_ERROR, "Connexion a la base impossible."); return; }

    char *cne = echapper(conn, champ(app, IDX_CNE));
    char *q = g_strdup_printf("DELETE FROM students WHERE cne='%s'", cne);

    if (mysql_query(conn, q))
        message(app, GTK_MESSAGE_ERROR, mysql_error(conn));
    else if (mysql_affected_rows(conn) == 0)
        message(app, GTK_MESSAGE_WARNING, "Aucun etudiant avec ce CNE.");
    else
        message(app, GTK_MESSAGE_INFO, "Etudiant supprime avec succes.");

    g_free(q); g_free(cne);
    mysql_close(conn);
}

static void executer_select(AppWidgets *app, const char *query) {
    MYSQL *conn = connecter_bd();
    if (!conn) { message(app, GTK_MESSAGE_ERROR, "Connexion a la base impossible."); return; }
    if (mysql_query(conn, query)) {
        message(app, GTK_MESSAGE_ERROR, mysql_error(conn));
    } else {
        MYSQL_RES *res = mysql_store_result(conn);
        if (res) { afficher_resultat(app, res); mysql_free_result(res); }
    }
    mysql_close(conn);
}

static void afficher(GtkWidget *w, gpointer data) {
    executer_select(data,
        "SELECT nom, prenom, cne, filiere, annee_etudes FROM students ORDER BY nom, prenom");
}

/* Recherche par CNE (exact) s'il est rempli, sinon par nom (partiel). */
static void rechercher(GtkWidget *w, gpointer data) {
    AppWidgets *app = data;
    MYSQL *conn = connecter_bd();
    if (!conn) { message(app, GTK_MESSAGE_ERROR, "Connexion a la base impossible."); return; }

    char *q;
    if (*champ(app, IDX_CNE)) {
        char *cne = echapper(conn, champ(app, IDX_CNE));
        q = g_strdup_printf("SELECT nom, prenom, cne, filiere, annee_etudes FROM students WHERE cne='%s'", cne);
        g_free(cne);
    } else if (*champ(app, 0)) {
        char *nom = echapper(conn, champ(app, 0));
        q = g_strdup_printf("SELECT nom, prenom, cne, filiere, annee_etudes FROM students WHERE nom LIKE '%%%s%%'", nom);
        g_free(nom);
    } else {
        mysql_close(conn);
        message(app, GTK_MESSAGE_WARNING, "Remplissez le CNE ou le nom pour rechercher.");
        return;
    }
    mysql_close(conn);
    executer_select(app, q);
    g_free(q);
}

/* ---------- Interface ---------- */

static void activate(GtkApplication *gapp, gpointer user_data) {
    AppWidgets *app = user_data;

    app->window = gtk_application_window_new(gapp);
    gtk_window_set_title(GTK_WINDOW(app->window), "Gestion des Etudiants - studium_db");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 700, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 12);
    gtk_container_add(GTK_CONTAINER(app->window), grid);

    for (int i = 0; i < NB_CHAMPS; i++) {
        GtkWidget *label = gtk_label_new(LABELS[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        app->entries[i] = gtk_entry_new();
        gtk_widget_set_hexpand(app->entries[i], TRUE);
        gtk_grid_attach(GTK_GRID(grid), label, 0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), app->entries[i], 1, i, 4, 1);
    }

    const char *noms[] = { "Ajouter", "Modifier", "Supprimer", "Rechercher", "Afficher tout" };
    GCallback actions[] = { G_CALLBACK(ajouter), G_CALLBACK(modifier), G_CALLBACK(supprimer),
                            G_CALLBACK(rechercher), G_CALLBACK(afficher) };
    for (int i = 0; i < 5; i++) {
        GtkWidget *btn = gtk_button_new_with_label(noms[i]);
        g_signal_connect(btn, "clicked", actions[i], app);
        gtk_grid_attach(GTK_GRID(grid), btn, i, NB_CHAMPS, 1, 1);
    }

    app->output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(app->output), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), app->output);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, NB_CHAMPS + 1, 5, 1);

    gtk_widget_show_all(app->window);
}

int main(int argc, char **argv) {
    static AppWidgets app;   /* durée de vie = celle du programme */
    GtkApplication *gapp = gtk_application_new("org.studium.gestion_etudiants",
                                               G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(gapp, "activate", G_CALLBACK(activate), &app);
    int status = g_application_run(G_APPLICATION(gapp), argc, argv);
    g_object_unref(gapp);
    return status;
}

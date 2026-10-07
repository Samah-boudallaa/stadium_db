CREATE DATABASE IF NOT EXISTS studium_db
  CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE studium_db;

CREATE TABLE IF NOT EXISTS students (
  id                INT AUTO_INCREMENT PRIMARY KEY,
  nom               VARCHAR(100) NOT NULL,
  prenom            VARCHAR(100),
  cne               VARCHAR(20)  NOT NULL UNIQUE,
  date_naissance    DATE,
  ville             VARCHAR(100),
  adresse           VARCHAR(255),
  compte_academique VARCHAR(100),
  filiere           VARCHAR(100),
  annee_etudes      VARCHAR(20)
);

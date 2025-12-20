CREATE DATABASE IF NOT EXISTS IoT;
USE IoT;

CREATE TABLE IF NOT EXISTS sensores (
    id INT AUTO_INCREMENT PRIMARY KEY,
    temp_estufa FLOAT,
    umid_estufa FLOAT,
    temp_externa FLOAT,
    umid_externa FLOAT,
    solo_estufa INT,
    solo_externo INT,
    nivel_poco INT,
    ar_raw INT,
    ar_qualidade INT,
    ar_status VARCHAR(20),
    data_hora DATETIME
);
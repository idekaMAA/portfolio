SELECT * FROM users WHERE email IS NULL; --usuarios cuyo email es NULL
SELECT * FROM users WHERE email IS NOT NULL; --usuarios cuyo email NO es NULL
-----------------------------------------------------------------------------------------------------
SELECT name, surname, IFNULL(age, 0) AS 'ages' FROM users; --selecciona el nombre, apellido y edad de los usuarios, reemplazando NULL en edad por 0
--as para que no salga la condición como nombre de columna
SELECT * FROM users ORDER BY age; /*ordeno por edad de menor a mayor (por defecto)*/
SELECT * FROM users ORDER BY age DESC; /*ordeno por edad de mayor a menor*/
SELECT * FROM  users ORDER BY age ASC; /*ordeno por edad de menor a mayor*/
SELECT * FROM  users ORDER BY age ASC, name DESC; /*ordeno por edad de menor a mayor y por nombre de mayor a menor*/
SELECT * FROM users WHERE email = 'petta@gmail.com' ORDER BY age DESC; /*usuarios con email específico ordenados por edad de mayor a menor (si la busqueda no tiene espacio, no es necesario las comillas*/
SELECT name FROM users WHERE email = 'petta@gmail.com' ORDER BY age DESC; /*nombres de usuarios con email específico ordenados por edad de mayor a menor*/

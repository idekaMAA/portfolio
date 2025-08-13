SELECT * FROM users LIMIT 2;--los primeros 2 datos de la tabla users
SELECT * FROM users WHERE NOT email LIKE '%@gmail.com' LIMMIT 5; /*los primeros 5 usuarios cuyo email NO termina en gmail.com*/
SELECT * FROM users ORDER BY age DESC LIMIT 3; /*los 3 usuarios mayores de edad ordenados de mayor a menor*/
SELECT * FROM users WHERE age > 30 ORDER BY name ASC LIMIT 4; /*los primeros 4 usuarios mayores de 30 años ordenados por nombre de menor a mayor*/
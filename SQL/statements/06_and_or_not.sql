SELECT * FROM users WHERE NOT age > 30; /*usuarios que NO son mayores de 30 años*/
SELECT * FROM users WHERE NOT email LIKE '%@gmail.com'; /*usuarios cuyo email NO termina en gmail.com*/
SELECT * FROM users WHERE NOT name = 'John'; /*usuarios cuyo nombre NO es John*/
SELECT * FROM users WHERE NOT age > 30 AND email LIKE '%@%'; /*usuarios que NO son mayores de 30 años y cuyo email contiene un @*/
SELECT * FROM users WHERE age > 30 OR email LIKE '%@gmail.com'; /*usuarios mayores de 30 años O cuyo email termina en gmail.com*/

SELECT * FROM users WHERE email LIKE '@gmail.com'; /*usuarios con email que termina en gmail.com*/
SELECT * FROM users WHERE email LIKE '%@gmail.com'; /*usuarios con email que termina en gmail.com, el % indica que puede haber cualquier cosa antes del @*/
--el % es para la búsqueda dinámica, puede haber cualquier cosa antes o después del símbolo que lo precede
SELECT * FROM users WHERE email LIKE 'petta%'; --usuarios con email que comienza con "petta"

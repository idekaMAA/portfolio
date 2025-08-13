SELECT COUNT(age) FROM users HAVING COUNT(age) > 0; --cuenta la cantidad de usuarios por edad y muestra solo las edades que tienen más de 0 usuarios
SELECT COUNT(age) FROM users HAVING COUNT(age) > 2; --cuenta la cantidad de usuarios por edad y muestra solo las edades que tienen más de 2 usuarios
--solo muestra si existen mas de 2 usuarios con las edades q cumplen la condicion
--no se puede usar WHERE en este caso porque WHERE filtra filas antes de la agrupación, mientras que HAVING filtra grupos después de la agrupación
-- HAVING se usa siempre con GROUP BY
SELECT name , init_date AS 'Fecha de inicio en programación' FROM users; --selecciona el nombre y la fecha de inicio de programación de los usuarios
--también se puede usar con comillas dobles
--el alias es para referirse a la columna de una forma más amigable (ponele)
SELECT CONCAT('Nombres: ', name, ' || Apellidos: ', surname) AS 'Nombre completo' FROM users; --concatena el nombre y el apellido de los usuarios con un alias
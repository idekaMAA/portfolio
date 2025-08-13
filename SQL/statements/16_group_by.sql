SELECT COUNT(age) , age FROM users GROUP BY age; --cuenta la cantidad de usuarios por edad
SELECT COUNT(age) FROM users GROUP BY age;-- cuenta la cantidad de usuarios por edad sin mostrar la edad
SELECT MAX(age) , age FROM users GROUP BY age;-- muestra la edad máxima de los usuarios por edad
SELECT COUNT(age) , age FROM users GROUP BY age ORDER BY age; -- cuenta la cantidad de usuarios por edad y ordena los resultados por edad
SELECT COUNT(age) , age FROM users GROUP BY age ORDER BY age ASC; -- cuenta la cantidad de usuarios por edad y ordena los resultados por edad de menor a mayor
SELECT COUNT(age) , age FROM users WHERE age > 23 GROUP BY age ORDER BY age ASC; -- cuenta la cantidad de usuarios mayores a 23 años por edad y ordena los resultados por edad de menor a mayor


SELECT * ,
CASE
	WHEN age > 17 THEN true
	ELSE false
END AS '¿Mayor de edad?'
FROM  users;

---------------------------------------------------------------------
SELECT * ,
CASE
	WHEN age > 17 THEN 'Es mayor de edad'
	ELSE 'Es menor de edad'
END AS '¿Mayor de edad?'
FROM  users;

-- el CASE es una estructura de control que permite evaluar condiciones y devolver un valor basado en esas condiciones
--puede tener mas de un WHEN
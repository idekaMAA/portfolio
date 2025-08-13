SELECT * FROM users WHERE name IN ('alexis');--usuarios cuyo nombre es alexis
SELECT * FROM users WHERE name NOT IN ('alexis');--usuarios cuyo nombre NO es alexis
SELECT * FROM users WHERE name IN ('alexis', 'john');--usuarios cuyo nombre es alexis o john
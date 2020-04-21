


CREATE TABLE IF NOT EXISTS slides(
	id						INTEGER					PRIMARY KEY AUTOINCREMENT,
	author_name		VARCHAR(64),
	content				TEXT						NOT NULL,	
	content_type	VARCHAR(64),	
	access_token	VARCHAR(128)		NOT NULL,
	edit_code			VARCHAR(128)		NOT NULL,
	create_time		INTEGER					NOT NULL
);


CREATE UNIQUE INDEX slides_tkn_idx on slides (access_token);


#for test.
DROP INDEX index_name;






package main

import (
	"database/sql"
	"database/sql/driver"
	"fmt"
	"log"
	"time"

	 "github.com/mattn/go-sqlite3"
)

type SQLWrapper struct{
		db *sql.DB,
}

func Open(file string) (*SQLWrapper, err error) {
	  db, err := sql.Open("sqlite3", "./foo.db")
	  if err != nil {
		  return (nil, err)
	  }
    wrapper := SQLWrapper{
			db: db
		}
		return (&wrapper, nil)
}

func (wrapper *SQLWrapper) Exec(query string, args ... interface{}) error {
	var err error
	for i:=0; i < 10; i++ {
			_, err := wrapper.db.Exec(query, args)
			if(err == sqlite3.ErrBusy) {
				time.Sleep(100 * time.Millisecond)
				continue
			} else {
				break
			}
	}
	if err != nil {
		fmt.Printf("%q: %s\n", err, query)
		return err
	} else {
		return nil
	}
}

func (wrapper *SQLWrapper) QueryRow(query string, record interface{}, args ... interface{}) error {
	var err error
	for i:=0; i < 10; i++ {
		 row, err := wrapper.db.QueryRow(query, args)
			if(err == sqlite3.ErrBusy) {
				time.Sleep(100 * time.Millisecond)
				continue
			} else {
				break
			}
	}
	if err != nil {
		fmt.Printf("%q: %s\n", err, query)
		return err
	} else {
		s := reflect.ValueOf(record).Elem()
		i:= s.NumField()
		f := make([]interface, i)
		row.Scan(f)
		for i:=0; i < s.NumField(); i++ {
			s.Field(i).Set(reflect.ValueOf(f[i]))
		}

		return nil
	}
}

func (wrapper *SQLWrapper) Query(query string, records interface{}, args ... interface{}) error {
	var err error
	for i:=0; i < 10; i++ {
		 rows, err := wrapper.db.Query(query, args)
			if(err == sqlite3.ErrBusy) {
				time.Sleep(100 * time.Millisecond)
				continue
			} else {
				break
			}
	}
	if err != nil {
		fmt.Printf("%q: %s\n", err, query)
		return err
	} else {
		s := reflect.ValueOf(row).Elem()
		i:= s.NumField()
		f := make([]interface, i)
		row.Scan(f)
		for i:=0; i < s.NumField(); i++ {
			s.Field(i).Set(reflect.ValueOf(f[i]))
		}

	defer rows.Close()
	for rows.Next() {
		var id int
		var name string
		err = rows.Scan(&id, &name)
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println(id, name)
	}
	err = rows.Err()
	if err != nil {
		log.Fatal(err)
	}


		return nil
	}
}

func (wrapper *SQLWrapper) Close() {
	wrapper.db.Close()
}

func SqlTest() {
	//os.Remove("./foo.db")

	db, err := sql.Open("sqlite3", "./foo.db")
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	sqlStmt := `
	create table IF NOT EXISTS foo (id integer not null primary key, name text);
	delete from foo;
	`
	_, err = db.Exec(sqlStmt)
	if err != nil {
		log.Printf("%q: %s\n", err, sqlStmt)
		return
	}

	tx, err := db.Begin()
	if err != nil {
		log.Fatal(err)
	}
	stmt, err := tx.Prepare("insert into foo(id, name) values(?, ?)")
	if err != nil {
		log.Fatal(err)
	}
	defer stmt.Close()
	for i := 0; i < 100; i++ {
		_, err = stmt.Exec(i, fmt.Sprintf("こんにちわ世界%03d", i))
		if err != nil {
			log.Fatal(err)
		}
	}
	tx.Commit()

	rows, err := db.Query("select id, name from foo")
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()
	for rows.Next() {
		var id int
		var name string
		err = rows.Scan(&id, &name)
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println(id, name)
	}
	err = rows.Err()
	if err != nil {
		log.Fatal(err)
	}

	stmt, err = db.Prepare("select name from foo where id = ?")
	if err != nil {
		log.Fatal(err)
	}
	defer stmt.Close()
	var name string
	err = stmt.QueryRow("3").Scan(&name)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(name)

	_, err = db.Exec("delete from foo")
	if err != nil {
		log.Fatal(err)
	}

	_, err = db.Exec("insert into foo(id, name) values(1, 'foo'), (2, 'bar'), (3, 'baz')")
	if err != nil {

		log.Fatal(err)
	}

	rows, err = db.Query("select id, name from foo")
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()
	for rows.Next() {
		var id int
		var name string
		err = rows.Scan(&id, &name)
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println(id, name)
	}
	err = rows.Err()
	if err != nil {
		log.Fatal(err)
	}
}

package main

import (
	"database/sql"
	"log"
	"sync"

	"github.com/gofrs/uuid"
)

var lck sync.Mutex

// HaicamOpenDb func
func HaicamOpenDb() (*sql.DB, error) {
	db, err := sql.Open("sqlite3", "./haicambox.db")
	if err != nil {
		log.Fatal("sql.Open: ", err)
	}

	return db, err
}

// HaicamInit func
func HaicamInit() {

	db, err := HaicamOpenDb()
	if err != nil {
		log.Fatal("sql.Open: ", err)
	}
	defer db.Close()

	/*query := `
	CREATE TABLE IF NOT EXISTS foo (id integer not null primary key, name text, amount real, file blob);
	CREATE INDEX IF NOT EXISTS name ON foo (name);
	DELETE FROM foo;
	DROP TABLE IF EXISTS foo;
	SELECT strftime('%s','now'); // timestamp now
	SELECT datetime('now'); // datetime now
	//INSERT OR IGNORE INTO Book(ID) VALUES(1001);
	//UPDATE Book SET Name='SQLite' WHERE ID=1001;
	func Sprintf(format string, a ...interface{}) string
	`*/

	//camera.event_type 0 - not support, 1 pull, 2 base, 3 in video stream
	//ip_pool: 1 - hub, 2-200 - cameras, others reserved
	//setting.key private_ip_prefix 172.(21-31).0. or 10.(21-254).0., data_version 1
	query := `
	CREATE TABLE IF NOT EXISTS setting ('key' text not null primary key, value text, last_update integer, created integer);
	CREATE TABLE IF NOT EXISTS ip_pool (ip_num integer not null primary key, used integer, last_update integer, created integer);
	CREATE TABLE IF NOT EXISTS camera (uuid text not null primary key, system_id integer, onvif_username text, onvif_password text, onvif_urn text, mac_address text, brand text, model text, local_ip text, private_ip text, onvif_port text, is_private integer, is_dhcp, is_onvif integer, onvif_endpoint, event_type integer, is_online integer, online_since integer, offline_since integer, parameters text,last_update integer, created integer)
	`
	_, err = db.Exec(query)

	if err != nil {
		log.Printf("%q: %s\n", err, query)
	}
}

// FoundHikvisionOrDahua func
func FoundHikvisionOrDahua(brand string, mac string, ipv4 string, port int, dhcp bool) {
	lck.Lock()
	defer lck.Unlock()

	db, err := HaicamOpenDb()
	if err != nil {
		log.Fatal("DB Open Err: ", err)
	}
	defer db.Close()

	tx, err := db.Begin()
	if err != nil {
		log.Fatal(err)
	}

	rows, err := tx.Query("select * from camera where mac_address = ? limit 1", mac)
	if err != nil {
		log.Fatal("Query err:", err)
	}

	hasRow := rows.Next()
	rows.Close()

	if hasRow {
		_, err = tx.Exec("update camera set local_ip = ?, onvif_port = ?, is_dhcp = ? where mac_address = ?", ipv4, port, dhcp, mac)
	} else {
		uuidV4 := uuid.Must(uuid.NewV4()).String()
		_, err = tx.Exec("insert into camera (brand, uuid, mac_address, local_ip, onvif_port, is_dhcp) values (?,?,?,?,?,?)", brand, uuidV4, mac, ipv4, port, dhcp)
	}

	if err != nil {
		log.Fatal("Exec err:", err)
	}

	tx.Commit()
}

// FoundOnvifCamera func
func FoundOnvifCamera(urn string, brand string, model string, ipv4 string, port int, endpoint string) {
	lck.Lock()
	defer lck.Unlock()

	db, err := HaicamOpenDb()
	if err != nil {
		log.Fatal("DB Open Err: ", err)
	}
	defer db.Close()

	tx, err := db.Begin()
	if err != nil {
		log.Fatal(err)
	}

	rows, err := tx.Query("select * from camera where onvif_urn = ? limit 1", urn)
	if err != nil {
		log.Fatal("Query err:", err)
	}

	hasRow := rows.Next()
	rows.Close()

	if hasRow {
		_, err = tx.Exec("update camera set brand = ?, model = ?, local_ip = ?, onvif_port = ?, onvif_endpoint = ? where onvif_urn = ?", brand, model, ipv4, port, endpoint, urn)
	} else {
		uuidV4 := uuid.Must(uuid.NewV4()).String()
		_, err = tx.Exec("insert into camera (uuid, onvif_urn, brand, model, local_ip, onvif_port, onvif_endpoint) values (?,?,?,?,?,?,?)", uuidV4, urn, brand, model, ipv4, port, endpoint)
	}

	if err != nil {
		log.Fatal("Exec err:", err)
	}

	tx.Commit()
}

package main 

import(
    "fmt"
    "os"
    "bufio"
    "net"
    "strconv"
	"io"
)

const (
    CONN_HOST = "localhost"
    CONN_TYPE = "tcp"

)
func main(){
 

    tcp_port := getPort()
	fmt.Println("tcpport:",tcp_port)
    listener, err := net.Listen(CONN_TYPE, ":"+strconv.FormatUint(uint64(tcp_port),10))
    if( err!= nil){
        fmt.Println("Error listening:",err.Error())
        os.Exit(1)
    }

    defer listener.Close()
    
    fmt.Println("This Server listening on :",strconv.FormatUint(uint64(tcp_port),10))

   for{
		fmt.Println("waitng for client...")
		new_conn, err := listener.Accept()
        if err != nil {
             fmt.Println("Error Accepting:",err.Error())
            os.Exit(1)
        }else{
        
        fmt.Println("Handle for ",new_conn.LocalAddr()," start.")
        go ClientHandler(new_conn)

        
        }
   
   } 
    
}

func getPort() uint16{
     std_reader := bufio.NewReader(os.Stdin)
   
    var tmp_bytes []byte
    fmt.Println("Input TCP Port to be used :")
    tmp_bytes, _ = std_reader.ReadBytes('\n')
   
    var num uint16
    i := 0
    for {
        fmt.Println(tmp_bytes[i])
		if(tmp_bytes[i]==10 || tmp_bytes[i] == 13) {
            return num
        }else{
            num= num*10 + uint16(tmp_bytes[i]-'0')
        }
        i++
    }

    
}
//go docs : fmt, bufio, net, os
func ClientHandler(con net.Conn) {
//   var str string
	recvBuf := make([]byte,1024)
  n, _ := con.Read(recvBuf[:])
   choice_made := string(recvBuf[:n]) 
   fmt.Println("Choise : ",choice_made)
	if choice_made == "1" {
	//request for download
		fmt.Println("Download Request recv")
		nn, _ := con.Read(recvBuf[:])

		file_name := string(recvBuf[:nn])
		file, err := os.Open("storage/"+file_name)
			if err != nil {
				fmt.Println("Error read file:",file_name)
				con.Close()
				return
			}else{
				defer file.Close()
				fmt.Println("Download Start for :",file_name)
				n, err  := io.Copy(con,file)
				if( err != nil){
				fmt.Println("Error send file")
				return
				}
			fmt.Println("Successfully sent :",n)
			con.Close()
			}
		return
	}else if choice_made == "2" {
	fmt.Println("Upload Request recv")
	nn, _ := con.Read(recvBuf[:])
	file_name := string(recvBuf[:nn])
	fmt.Println("FILENAME : ",file_name)
	file, err := os.Create("storage/"+file_name)
		if( err!= nil ){
			fmt.Println("Error creating file ", file_name)
			con.Close()

		}else{
			defer file.Close()
			fmt.Println("Upload Start for :",file_name)
			_, err := io.Copy(file,con)
			if(err != nil){
				fmt.Println("Error recv file")
				return
			}

			fmt.Println("Successfully uploaded for : ",file_name)
			con.Close()
		}
	}
}

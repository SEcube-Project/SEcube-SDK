# Requirements Document 
## SEFatFs
Authors: D. Stochino, M. Meloni


# Contents

- [Stakeholders](#stakeholders)
- [Context Diagram and interfaces](#context-diagram-and-interfaces)
	+ [Context Diagram](#context-diagram)
	+ [Interfaces](#interfaces) 
	
- [Stories and personas](#stories-and-personas)
- [Functional and non functional requirements](#functional-and-non-functional-requirements)
	+ [Functional Requirements](#functional-requirements)
	+ [Non functional requirements](#non-functional-requirements)
- [Use cases](#use-cases)
- [Glossary](#glossary)
- [System design](#system-design)


# Stakeholders


| Stakeholder name  | Description | 
| ----------------- |:-----------:|
|User               |A developer of the SECube firmware|
|SDcard       |Provides the storage for the application files|

# Context Diagram and interfaces


## Context Diagram


```plantuml
left to right direction
actor User as u
actor SDcard as mp
u -- (SEFatFs)
mp --(SEFatFs)
```

## Interfaces

| Actor | Logical Interface | Physical Interface  |
| ------------- |:-------------:| -----:|
| SDcard  | read/write primitives     | SDIO |  
| User | [SEFatFs APIs](#apis) |-|

# Stories and personas
Nico, a SECube firmware developer, wants to store a large database of keys inside the SECube platform. Unfortunately, the internal flash memory of SEcube is too small. Moreover, most of the available database systems for embedded systems require a properly structured file system in order to function. The SDCard slot of the SECube can be exploited for this purpose but, since it is easy to remove the SDcard from the secube and plug it into a common PC, the keys stored inside can be easly leaked aswell. SEFatFs provides file primitives that can be utilized to create secure files that give both confidentiality and integrity, making use of cryptograpfic functions.

# Functional and non functional requirements

## Functional Requirements

| ID        | Description  |
| ------------- |:-------------:| 
|FR1   | Open a secure file|
|FR1.1 | Open an existing file|
|FR1.2 | Create a new file|
|FR2   | Write data inside an open file|
|FR2.1 | Expand the file with new data|
|FR2.2 | Modify existing data of a file|
|FR3   | Read data from an open file|
|FR4   | Manage secure directories|
|FR4.1 | Create a directory|
|FR4.2 | Open a directory|
|FR4.3 | Read directory items|


## Non Functional Requirements


| ID        | Type        | Description  | Refers to |
| ------------- |:-------------:| :-----:| -----:|
|  NFR1     |Security |-  |All FR |  
|  NFR1.1     |Confidentiality |Secure files must be accessible only by the SECube that contains the corresponding key |All FR |
|  NFR1.2     |Integrity    |Any modification (intentional or unintentional) of a secure file must be detected| FR1.1, FR3 |
|  NFR2		|Usability| A developer which knows how to use fat_fs shall be able to use SEfat_fs with no training| All FR |


## Use cases

### Use case 1, UC1 - FR1.1 Open existing file

| Actors Involved        | User, SDcard |
| ------------- |:-------------:|
|  Precondition     | Secure file SF exists in SDcard|  
|  Post condition     | File SF is ready to be read/written|
|  Nominal Scenario     | The user U uses the API secure_open specifying the path of the file and the opening mode|
|  Variants     |File SF does not exists in SDcard, return error <br /> File SF has been illecitly modified, return error|

### Use case 2, UC2 - FR1.2 Create a new file

| Actors Involved        | User, SDcard |
| ------------- |:-------------:|
|  Precondition     | Secure file SF does not exists in SDcard. Key K and algorithm exist in SECube|  
|  Post condition     | File SF is ready to be read/written|
|  Nominal Scenario     | The user U uses the API secure_open specifying the path of the file, the opening mode, the key K and the algorithm A|
|  Variants     |File SF already exists in SDcard, return error <br />File SF already exists in SDcard, but user specified to overwrite it, return OK <br /> The key K or the algorithm A do not exist inside SECube, return error|

### Use case 3, UC3 - FR2 Write data inside an open file

| Actors Involved        | User, SDcard |
| ------------- |:-------------:|
|  Precondition     | Secure file SF is open|  
|  Post condition     | The modifications appllied to SF are written on the SDcard|
|  Nominal Scenario     | The user U uses the API secure_write to write data in the file SF buffer <br/> The user U uses the API secure_close to write all the pending modifications in the SDcard |
|  Variants     |File SF is not open in write mode, return error <br />The user U uses API secure_seek in order to write at a specific location of the file SF|


### Use case 4, UC4 - FR3 Read data from an open file

| Actors Involved        | User, SDcard |
| ------------- |:-------------:|
|  Precondition     | Secure file SF is open|  
|  Post condition     | Data are read by the User U|
|  Nominal Scenario     | The user U uses the API secure_read to read data from the file SF |
|  Variants     |File SF is not open in read mode, return error <br />The user U uses API secure_seek in order to read from a specific location of the file SF|


# Glossary
```plantuml
class SEFatFs{

}

class Secure_item{
	path
	algorithm
	key
}

class SE3_FIL {
	pointer
}

class SE3_DIR {

}


SEFatFs "1"---"*" Secure_item
SE3_DIR "1"---"*" Secure_item
Secure_item <|---  SE3_FIL
Secure_item <|---  SE3_DIR

```

# System Design

```plantuml

left to right direction
package "SECube" {
	[SECube firmware]
	[FatFs driver]
	[SEFatFs]
	[SDIO driver]
}

[SECube firmware] <-> [SEFatFs]
[SECube firmware] <-> [FatFs driver]
[SEFatFs] <-> [FatFs driver]
[FatFs driver] <-> [SDIO driver]

[SDcard] <-> [SDIO driver]
```

# APIs
	SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, BYTE mode, uint32_t keyID, uint16_t algo);

	SE3_FRESULT secure_close(SE3_FIL* se_fp);

	SE3_FRESULT secure_read(SE3_FIL* se_fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead);

	SE3_FRESULT secure_write(SE3_FIL* se_fp, uint8_t *dataIn, uint32_t dataIn_len);

	SE3_FRESULT secure_seek(SE3_FIL* se_fp, int64_t offset, int32_t *position, uint8_t whence);

	SE3_FRESULT secure_mkdir(char *path, uint32_t keyID, uint16_t algo);

	SE3_FRESULT secure_opendir(SE3_DIR* se_dp, char *path);

	SE3_FRESULT secure_readdir (SE3_DIR* se_dp, SE3_FILINFO* fno);

	SE3_FRESULT secure_closedir (SE3_DIR* se_dp);
	



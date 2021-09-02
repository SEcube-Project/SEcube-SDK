# Requirements Document 

Authors: D. Castagneri, I. G. Foderini, T. Madeo, D. Stochino

Date: 19/04/2020

Version: 1

# Contents

- [Stakeholders](#stakeholders)
- [Context Diagram and interfaces](#context-diagram-and-interfaces)
	+ [Context Diagram](#context-diagram)
	+ [Interfaces](#interfaces) 
	
- [Stories and personas](#stories-and-personas)
- [Functional and non functional requirements](#functional-and-non-functional-requirements)
	+ [Functional Requirements](#functional-requirements)
	+ [Non functional requirements](#non-functional-requirements)
- [Use case diagram and use cases](#use-case-diagram-and-use-cases)
	+ [Use case diagram](#use-case-diagram)
	+ [Use cases](#use-cases)
- [Glossary](#glossary)
- [System design](#system-design)
- [Deployment diagram](#deployment-diagram)


# Stakeholders


| Stakeholder name  | Description | 
| ----------------- |:-----------:|
|User               |Uses the application to find the best gas station that fit his needs|
|Developer          |Develop the application and maintains it by fixing bugs with the help of user reports|
|Open street maps       |Provides the system informations about driving directions|

# Context Diagram and interfaces


## Context Diagram


```plantuml
left to right direction
actor User as u
actor OpenStreetMap as mp
u -- (EzGas)
mp --(EzGas)
```

## Interfaces

| Actor | Logical Interface | Physical Interface  |
| ------------- |:-------------:| -----:|
| Open street maps  | REST API v 0.6     | Internet network |  
| User | GUI |Smartphone, PC|

# Stories and personas
John Cage, 18, student.

As a novice driver, John receives his first car as a gift. Since his parents want him to gain responsability in managing money, they ask him to cover fuel cost. John would be very happy to use an app to always find the cheapest gas station.

Alice Green, 29, saleswoman.

As a cosmetic agent, Alice's job consist in driving a lot to promote and selling make-up products. She makes around 500 kilometers per week. In order to save money and reduce her carbon footprint, she has recently bought a methane gas powered car. Because of the less popularity of methane gas stations and the need of the attendant to fuel, she wants an app to plan her trips without running out of gas.

# Functional and non functional requirements

## Functional Requirements

| ID        | Description  |
| ------------- |:-------------:| 
|FR1   | List gas stations|
|FR1.1 | Sort gas stations by price|
|FR1.2 | Sort gas stations by distance|
|FR1.3 | Filter gas stations by fuel type|
|FR2   | Collect user submitted reports|
|FR3   | Manage Accounts|
|FR3.1 | Sign in|
|FR3.2 | Log in|
|FR3.3 | Log out|
|FR3.4 | Remove account|
|FR4   | Manage price evaluations|
|FR4.1 | Collect feedbacks|
|FR4.2 | Update reporting user's "feedback rating"|
|FR5   | Show driving directions from user position to gas station position|
|FR6   | Manage prices and their accuracy|
|FR6.1 | Compute prices from price reports|
|FR6.2 | Compute accuracy from the reporting users "feedback rating"|
|FR7   | Manage gas stations |
|FR7.1 | Insert gas stations with user reports|
|FR7.2 | Delete gas stations with user reports|

## Non Functional Requirements


| ID        | Type (efficiency, reliability, ..)           | Description  | Refers to |
| ------------- |:-------------:| :-----:| -----:|
|  NFR1     |Efficiency |All functions should complete in < 0.5 sec  |All FR |
|NFR2|Portability|The Application should be able to run on the latest version of the most used desktop and mobile web browsers: Chrome (80.0 and latest), Firefox (70.0 and latest), Opera (36.0 and latest), Safari (9.1 and latest), Edge (75.0 and latest) |All FR|  
|  NFR3     |Usability |Application should be used with no training by users |All FR |
|  NFR4     |Privacy    |All data of a user should be accesible to the user only| FR3 |
|  NFR5		|Localisation|Decimal numbers use . (dot) as decimal separator | FR1 |

# Use case diagram and use cases

## Use case diagram


```plantuml
left to right direction
actor User as u
actor OpenStreetMap as mp
u --> (FR1 Find gas stations)
u --> (FR2 Submit report)
u --> (FR3.1 Create account)
u --> (FR3.2 Log in)
u --> (FR3.3 Log out)
u --> (FR3.4 Remove account)
u --> (FR4 Send prices feedback)
u --> (FR5 See directions to chosen gas station)

(FR5 See directions to chosen gas station) --> mp
(FR1 Find gas stations) --> mp

```

## Use cases

### Use case 1, UC1 - FR1 Find gas stations

| Actors Involved        | User, Map provider |
| ------------- |:-------------:|
|  Precondition     | |  
|  Post condition     | |
|  Nominal Scenario     | User U inserts his/her current location, selects fuel type, selects max distance, selects a gas station G, sees fuel prices|
|  Variants     |No gas station available with selected filters, issue a warning|

##### Scenario 1.1

| Scenario 1.1 | |
| ------------- |:-------------:|
|  Precondition     |A gas station G that respects the filters(fuel type, max distance) exists|
|  Post condition     | |
| Step#        | Description  |
|  1     | User inserts current location |
|  3     | User inserts a max distance|
|  4     | User selects a fuel type|
|  5     | Map Provider computes distances|
|  6     | User selects gas station G |

##### Scenario 1.2

| Scenario 1.2 | |
| ------------- |:-------------:|
|  Precondition     |A gas station G that respects the filters(fuel type, max distance) doesn't exist  |
|  Post condition     | |
| Step#        | Description  |
|  1     | User inserts current location |  
|  2     | User inserts a max distance|
|  3     | No gas station found, issue a warning |

### Use case 2, UC2 - FR2 Submit report

| Actors Involved        | User |
| ------------- |:-------------:|
|  Precondition     |User U is logged in Account A, gas station G exists|  
|  Post condition     |Report R related to gas station G and account A is submited|
|  Nominal Scenario     |User U find a gas station G and submits a report (can be deletion or price report)|
|  Variants     |The gas station G doesn't already exist and the user wants to report it as a new gas station (insertion report)|

##### Scenario 2.1 

| Scenario 2.1 | |
| ------------- |:-------------:|
|  Precondition     |User U is logged in account A, gas station G exists|
|  Post condition     | Report of type price report P is submitted|
| Step#        | Description  |
|  1     | User U selects gas stations G |
|  2     | User U inserts new prices|
|  3     | User U submits the price report P|

##### Scenario 2.2 

| Scenario 2.2 | |
| ------------- |:-------------:|
|  Precondition     |User U is logged in account A, gas station G exists |
|  Post condition     | Report of type deletion report D is submitted|
| Step#        | Description  |
|  1     | User U selects gas station G |
|  2     | User U submits the deletion report D|

##### Scenario 2.3 

| Scenario 2.3 | |
| ------------- |:-------------:|
|  Precondition     |User U is logged in account A, gas station G doesn't exist |
|  Post condition     | Report of type insertion report is submitted (which is not related to any existing gas station)|
| Step#        | Description  |
|  1     | User U insert information(address, name) of the new gas station|
|  2     | User U submits the insertion report|


### Use case 3, UC3 - FR3.1 Create account

| Actors Involved        |User  |
| ------------- |:-------------:|
|  Precondition     |  |  
|  Post condition     | Account A exists |
|  Nominal Scenario     | User U inserts his credentials(username, email, password), account his created |
|  Variants     |Email or username already used, issue warning |

##### Scenario 3.1 

| Scenario 3.1 |   |
| ------------- |:-------------:|
|  Precondition     |Username and email inserted by the user aren't already used|
|  Post condition     |Account A created  |
| Step#        | Description  |
|  1     |User U inserts his credentials (username, email and password)|  
|  2     |Check if username or email are already used|
|  3     |Notificate U that his Account A has been created|

##### Scenario 3.2 
| Scenario 3.2 |   |
| ------------- |:-------------:|
|  Precondition     |Username or email are already used  |
|  Post condition     |Account A not created |
| Step#        | Description  |
|  1     |User U inserts his credentials |  
|  2     |Check if username or email are already used|
|  3     |Notificate U that his Account A has not been created|

### Use case 4, UC4 - FR3.2 Log in

| Actors Involved        |User  |
| ------------- |:-------------:|
|  Precondition     | Account exists |  
|  Post condition     |  |
|  Nominal Scenario     | User U inserts his credentials (username, password) to log in his account |
|  Variants     |No correspondence (username, password) found, issue a warning |

##### Scenario 4.1 

| Scenario 4.1 |   |
| ------------- |:-------------:|
|  Precondition     |Account A with credentials (email, password) inserted by the user exists|
|  Post condition     |User is logged in  |
| Step#        | Description  |
|  1     |User U inserts email and password|  
|  2     |Set user logged in account A|
|  3     |Notify user that he logged in|

##### Scenario 4.2 

| Scenario 4.2 |   |
| ------------- |:-------------:|
|  Precondition     |Account A with (email, password) inserted by the user doesn't exist|
|  Post condition     |User is not logged in  |
| Step#        | Description  |
|  1     |User U inserts email and password|  
|  2     |Notify User that log in failed|

### Use case 5, UC5 - FR3.3 Log out

| Actors Involved        |User  |
| ------------- |:-------------:|
|  Precondition     | Account A exists, User U is logged in |  
|  Post condition     |  |
|  Nominal Scenario     | User U logs out from Account A |
|  Variants     | |

##### Scenario 5.1 
| Scenario 5.1 |   |
| ------------- |:-------------:|
|  Precondition     |User U is logged in Account A|
|  Post condition     |User U is not logged in any account|
| Step#        | Description  |
|  1     |User U logs out from account A|  
|  2     |Notificate U that he logged out|


### Use case 6, UC6 - FR3.4 Remove account
| Actors Involved        |User  |
| ------------- |:-------------:|
|  Precondition     | Account A exists, User U is logged in Account A |  
|  Post condition     | Account A doesn't exist, User U is logged out |
|  Nominal Scenario     | The user removes his account |
|  Variants     | |

##### Scenario 6.1 
| Scenario 6.1 |   |
| ------------- |:-------------:|
|  Precondition     |User U is logged in account A |
|  Post condition     | |
| Step#        | Description  |
|  1     |User U removes Account A|  
|  2     |Notificate user that his account has been removed|

### Use case 7, UC7 - FR4 Send price feedback
| Actors Involved        |User  |
| ------------- |:-------------:| 
|  Precondition     | User U is logged in account A, price P exists, user S submitted a report for price P |  
|  Post condition     | S.account.feedbackRating is updated |
|  Nominal Scenario     | User U gives feedback to a price P, the submitter account S.account "feedback rating" is modified accordingly  |
|  Variants     |P.LastUpdate is not recent, S.account.feedbackRating is not updated, issue a warning |

##### Scenario 7.1 

| Scenario 7.1 |   |
| ------------- |:-------------:|
|  Precondition     |The price has been updated recently  |
|  Post condition     | Submitter account S.account "feedback rating" is updated |
| Step#        | Description  |
|  1     |User U gives a feedback to price P |  
|  2     |Select price report PR from P|
|  3     |Select submitter account S from PR |
|  4     |Update S.account "feedback rating"|

##### Scenario 7.2 

| Scenario 7.2 |   |
| ------------- |:-------------:|
|  Precondition     |The price hasn't been updated recently  |
|  Post condition     | Submitter account S.account "feedback rating" is not updated |
| Step#        | Description  |
|  1     |User U gives a feedback to price P |  
|  2     |Notificate U that the price isn't up to date|

### Use case 8, UC8 - FR5 See directions to chosen gas station

| Actors Involved        |User, Map Provider  |
| ------------- |:-------------:|
|  Precondition     |Gas Station G exists |  
|  Post condition     |  |
|  Nominal Scenario     | User U select G he wants to reach, Map Provider provides driving directions |
|  Variants     | |

##### Scenario 8.1 
| Scenario 8.1 |   |
| ------------- |:-------------:|
|  Precondition     |Gas Station G exists |
|  Post condition     |  |
| Step#        | Description  |
|  1     |User U selects a gas station G |  
|  2     |Map provider computes the path to reach G|
|  3     |The path is showed to the user|


# Glossary
```plantuml
class EzGas{

}

class Account {
	email address
	username
	password hash
	feedback rating
}

class GasStation {
	ID
	address
	name
}

class Price {
	price
	fuel: {gasoline, diesel, LPG, methane}
	last update
	accuracy
}

class Report {
	time tag
}

class PriceReport {
	fuel: {gasoline, diesel, LPG, methane}
	price
}


class InsertionReport {
	name
	address
}

class DeletionReport {

}

EzGas "1"---"*" Account
EzGas "1"---"*" GasStation
Account "1"---"*" Report : Submits
GasStation "1"---"0..4" Price
Account "*"---"*" Price : Gives feedback
GasStation "1"---"*" PriceReport
GasStation "1"---"*" DeletionReport
Report <|--- PriceReport
Report <|--- DeletionReport
Report <|---  InsertionReport

```

# System Design
Not meaningful in this case

# Deployment Diagram
```plantuml

node "Web Server" as a {
	artifact "Web Site"
}

node "User Device" as b {
	artifact "Web Browser"
}

node "Database Server" as c {
	artifact "SQL database"
}

node "Application Server" as d

a -- b
a -- d
d -up- c

```


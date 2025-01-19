# smart-iot-system
스마트 환경 제어 시스템 :desktop_computer:

## 목차
1. 개요
2. 프로젝트 설명
3. 구성도
   -	하드웨어 구성도
   -	소프트웨어 구성도
4.	순서도
5.	기능 구현 설명

## 개요
- 프로젝트 이름: 스마트 환경 제어 시스템
- 프로젝트 지속기간: 2024.12.06 ~ 2024.12.9
- 개발 언어: C
- 프로젝트 목표:
  <br />
  :heavy_check_mark: DHT11 센서를 활용하여 실시간으로 정확한 온습도 데이터를 측정.
  <br />
  :heavy_check_mark: 설정된 조건을 초과할 경우 서보모터를 자동 제어하여 팬 기능 수행.
  <br />
  :heavy_check_mark: I2C LCD를 통해 온습도 데이터를 실시간으로 표시하여 사용자 편의성을 증대
  <br />


## 구성도
- 하드웨어 구성도
<img width="458" alt="image" src="https://github.com/user-attachments/assets/9431a7b9-722c-4a02-b26c-145349948a5d" />
<br />

- 소프트웨어 구성도
<br />
<img width="338" alt="image" src="https://github.com/user-attachments/assets/d95e5811-051b-411f-84ec-e88c561c8c21" />

## 순서도
<img width="458" alt="image" src="https://github.com/user-attachments/assets/0a74efdb-f451-44fd-a2d4-bb130d8a583a" />

## 기능 구현 설명
1. DHT11 센서를 통한 온습도 측정
   - DHT11 센서를 사용하여 실시간으로 온도와 습도를 측정한다.
   
2. LCD를 통한 데이터 출력
   - 측정한 온도와 습도를 LCD를 통해 사용자에게 실시간으로 출력한다.
   - LCD 첫번째 줄에는 온도, 두번째 줄에는 습도를 표시한다.
     
3. 서보모터 제어
   - 온도 또는 습도가 설정된 임계값을 초과하면 서보모터가 작동, 팬처럼 회전한다.
   - 조건이 충족되지 않을 경우 서보모터는 정지한다.

<table>
  <tr>
    <td align="center">
      <img src="https://github.com/user-attachments/assets/4e51add6-05ba-4ef5-b393-1b678d8b1251" width="200" />
      <br />
      <span style="font-size: 70%;">습도 60% 이상 서보모터 동작</span>
    </td>
    <td align="center">
      <img src="https://github.com/user-attachments/assets/3fac7581-914e-4875-b2e2-ea26a2c88569" width="200" />
      <br />
      <span style="font-size: 70%;">습도 60% 이하 서보모터 정지</span>
    </td>
  </tr>
</table>




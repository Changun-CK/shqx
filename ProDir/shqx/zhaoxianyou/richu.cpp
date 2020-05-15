
object TimeUtil {
  def main(args: Array[String]): Unit = {
      println(getDay(121.07,31.03,1551424419))
  }
 
  /**
    * 计算当地日出日落UTC
    */
  def getDay(lon: Double, lat: Double, utc: Int): (Int, Int) = {
    var day = 0;
    var rc = 0;
    var rl = 0;
    val lon_pai = Math.PI * (lon / 360)
    val lat_pai = Math.PI * (lat / 360)
    val time_zone = getTimeZone(lon) //时区
 
    val time_real = utc + time_zone * 3600 // 真实时间，计算出所在时区的本地时间
    println(time_real)
    val yearTimeStamp = getUtcYearStart(time_zone, time_real.toString) // 获取所在时区年份的UTC
    val N = (utc - yearTimeStamp) / 86400 // 一年中的第几天
    val b = (2 * Math.PI * N) / 365
    val delta = 0.006918 - 0.399912 * math.cos(b) + 0.070257 * math.sin(b) - 0.006758 * math.cos(2 * b) + 0.000907 * math.sin(2 * b) - 0.002697 * math.cos(3 * b) + 0.00148 * math.sin(3 * b)
    val gama = Math.PI / 2 - math.abs(delta)
 
    // 判断极昼、极夜
    if (delta >= 0) {
      if (lat_pai > gama) {
        rc = 1
      } else if (lat_pai < -gama) {
        rl = 1
      }
    } else if (delta < 0) {
      if (lat_pai > gama) {
        rl = 1
      } else if (lat_pai < -gama) {
        rc = 1
      }
    }
    if (lat_pai >= -gama && lat_pai <= gama) {
      var time_day = 24 - 24 * Math.acos(Math.tan(delta) * Math.tan(lat_pai)) / Math.PI //昼长
      var time_z = 12 + (time_zone * 15 - lon) / 15 //太阳正午时间
      var time_days = time_real / 86400 // 计算当地天数
      var time_rc = ((time_z - time_day / 2) * 3600 + time_days * 86400).toInt - time_zone * 3600
      var time_rl = ((time_z + time_day / 2) * 3600 + time_days * 86400).toInt - time_zone * 3600
      rc = time_rc
      rl = time_rl
    }
    (rc, rl)
  }
 
  /**
    * 计算时区
    */
  def getTimeZone(lon: Double): Int = {
    var time_zone = 0
    if (lon > 0) {
      if (lon > 7.5) {
        time_zone = ((lon - 7.5) / 15 + 1).toInt
      }
    } else if (lon < 0) {
      if (lon < -7.5) {
        time_zone = ((lon + 7.5) / 15 - 1).toInt
      }
    }
    (time_zone)
  }
 
  /**
    * 获取所在时区当天开始时间的UTC
    * （获取所在时区的当天日期，补全开始时间，并转化为UTC）
    */
  def getUtcNightStart(lon: Double, rc: String): Long = {
    val time_zone = getTimeZone(lon)
    val fm = new SimpleDateFormat("yyyy-MM-dd")
    val timeDay = fm.format(new Date((rc.toLong - 8 * 3600 + time_zone * 3600) * 1000)) // 所在时区的当天日期
    val date = fm.parse(timeDay)
    val timeHour = date.getTime / 1000 // 所在时区的当天日期在东八区的当天日期的时间戳
    val nightStart = timeHour + 8 * 3600 - time_zone * 3600 // 所在时区的当天日期开始时间所对应的UTC
    nightStart
  }
 
  /**
    * 计算所在时区当年的开始UTC
    */
  def getUtcYearStart(time_zone: Int, tm: String): Long = {
    val fm = new SimpleDateFormat("yyyy")
    val timeYear = fm.format(new Date((tm.toLong - 8 * 3600 + time_zone * 3600) * 1000)) // 所在时区的年份
    val dt = fm.parse(timeYear)
    val timeHour = dt.getTime() / 1000
    val yearStart = timeHour + 8 * 3600 - time_zone * 3600
    yearStart
  }
}

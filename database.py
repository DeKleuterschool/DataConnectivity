from sqlalchemy import create_engine
from sqlalchemy.orm import declarative_base
from sqlalchemy import Integer, String, REAL, TEXT
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship, Session, sessionmaker
from sqlalchemy import exc
from sqlalchemy import and_
from sqlalchemy import func, extract
import datetime

base = declarative_base()
engine = create_engine('sqlite:///data.sqlite', connect_args={'check_same_thread': False})
SessionLocal = sessionmaker(bind=engine)

class Bewerking():
   def save(self):
      with SessionLocal() as session:
         session.add(self)
         session.commit()

class Bike(base, Bewerking):
    rowid = Column(Integer, primary_key=True)
    __tablename__='bike'
    lat = Column(REAL)
    lng = Column(REAL)
    date = Column()
    
    def __init__(self, lat, lng, date):
        self.lat = lat
        self.lng = lng
        self.date = date

def new_bike(lat, lng, date):
    bike = Bike(lat=lat, lng=lng, date=date)
    bike.save()

def get_bike(date):
    with SessionLocal() as session:
        bike = session.query(Bike).filter(Bike.date >= date).order_by(Bike.date.asc()).first()
        if bike:
            return {
                'lat': bike.lat,
                'lng': bike.lng,
                'date': bike.date
            }
        return None
    
if __name__ == "__main__":
    base.metadata.create_all(engine)

    new_bike(53.2194, 6.5665, datetime.datetime(2025, 1, 6, 14, 20))
    new_bike(54.2194, 6.5665, datetime.datetime(2025, 1, 6, 15, 20))
    new_bike(55.2194, 6.5665, datetime.datetime(2025, 1, 7, 16, 20))
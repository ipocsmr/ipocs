/*****************
 *  Object store
 *
 * @author Fredrik Elestedt
 * @date
 * @history see the git log of the origin repository.
 *
 */
#ifndef POINTSMOTOR_STORE_H
#define POINTSMOTOR_STORE_H

#include "PointsMotor.h"

class PointsMotorStore {
  public:
    static PointsMotorStore& getInstance()
    {
      static PointsMotorStore instance;
      return instance;
    }

    void registerType(int typeId, initPointsMotorFunction fun);
    void getFunction(int typeId, initPointsMotorFunction* fun);
  private:
    initPointsMotorFunction functions[2];
    PointsMotorStore();
    PointsMotorStore(PointsMotorStore const&);              // Don't Implement
    void operator=(PointsMotorStore const&); // Don't implement
};

#endif


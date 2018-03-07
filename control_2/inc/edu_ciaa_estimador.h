

#ifndef EDU_CIAA_ESTIMADOR_H_
#define EDU_CIAA_ESTIMADOR_H_

#include <chip.h>
#include <mpu9250.h>

// =============================================================================
typedef float   float32_t;
typedef double  float64_t;

typedef enum { FILTER_ERROR = 0, FILTER_OK } filter_status_t;

typedef struct
{
  uint16_t SampleRate;
  float32_t Weight;
} filter_init_t;

// =============================================================================
filter_status_t ciaa_estimator_init(filter_init_t *filter_InitStruct);
filter_status_t ciaa_estimator_notFilteredAngles(float32_t *pAngles);
filter_status_t ciaa_estimator_filteredAngles(float32_t *pAngles);

#endif //EDU_CIAA_ESTIMADOR_H_
// EOF =========================================================================

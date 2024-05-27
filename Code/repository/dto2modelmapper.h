#ifndef DTO2MODELMAPPER_H
#define DTO2MODELMAPPER_H

#include "singleton.h"

class Assignment;
class AssignmentDto;

class Assistant;
class AssistantDto;

class Consultation;
class ConsultationDto;

class Fcs;
class FcsDto;

class Grade;
class GradeDto;

class Student;
class StudentDto;

class Teacher;
class TeacherDto;

class Work;
class WorkDto;

class Dto2ModelMapper : public Singleton<Dto2ModelMapper>
{
public:
    Dto2ModelMapper();

    static Assignment *convertFromDto(const AssignmentDto &dto);
    static AssignmentDto convertFromModel(const Assignment &model);

    static Assistant *convertFromDto(const AssistantDto &assistantDto);
    static AssistantDto convertFromModel(const Assistant &assistant);

    static Consultation *convertFromDto(const ConsultationDto &consultationDto);
    static ConsultationDto convertFromModel(const Consultation &consultation);

    static Fcs *convertFromDto(const FcsDto &fcsDto);
    static FcsDto convertFromModel(const Fcs &fcs);

    static GradeDto convertFromModel(const Grade &grade);
    static Grade *convertFromDto(const GradeDto &gradeDto);

    static StudentDto convertFromModel(const Student &student);
    static Student *convertFromDto(const StudentDto &studentDto);

    static Teacher *convertFromDto(const TeacherDto &teacherDto);
    static TeacherDto convertFromModel(const Teacher &teacher);

    static Work *convertFromDto(const WorkDto &workDto);
    static WorkDto convertFromModel(const Work &work);
};

#endif // DTO2MODELMAPPER_H
